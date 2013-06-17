//========================================================================
//
// MCOutputDev.cc
//
// Copyright 2013 Igalia S.L.
//
//========================================================================

#include "MCOutputDev.h"
#include "GfxFont.h"
#include "GfxState.h"
#include "Annot.h"
#include "Link.h"
#include <vector>

struct MCOutputDev::Priv
{
  MCOpArray mcOps;
  bool      capturing;
  int       mcid;
  GfxFont  *lastFont;
  Guint     lastFlags;
  double    pageWidth;
  double    pageHeight;

  Priv(int mcidA):
    capturing(false),
    mcid(mcidA),
    lastFont(0),
    lastFlags(0),
    pageWidth(0.0),
    pageHeight(0.0)
  {}
};


MCOutputDev::MCOutputDev(int mcid):
  p(new Priv(mcid))
{
}


MCOutputDev::~MCOutputDev()
{
  delete p;
}


void MCOutputDev::startPage(int pageNum, GfxState *state, XRef *xref)
{
  if (state) {
    p->pageWidth  = state->getPageWidth();
    p->pageHeight = state->getPageHeight();
  } else {
    p->pageWidth = p->pageHeight = 0.0;
  }
}


void MCOutputDev::endPage()
{
  p->pageWidth = p->pageHeight = 0.0;
}


void MCOutputDev::beginMarkedContent(char *name, Dict *properties)
{
  int id = -1;
  if (properties && properties->lookupInt("MCID", NULL, &id) && id == p->mcid)
    p->capturing = true;
}


void MCOutputDev::endMarkedContent(GfxState *state)
{
  p->capturing = false;
}


void MCOutputDev::drawChar(GfxState *state,
                           double xx, double yy,
                           double dx, double dy,
                           double ox, double oy,
                           CharCode c, int nBytes,
                           Unicode *u, int uLen)
{
  if (!p->capturing || !uLen)
    return;

  double sp, dx2, dy2, w1, h1, x1, y1;

  // Subtract char and word spacing from the (dx,dy) values
  sp = state->getCharSpace();
  if (c == (CharCode) 0x20)
    sp += state->getWordSpace();
  state->textTransformDelta(sp * state->getHorizScaling(), 0, &dx2, &dy2);
  dx -= dx2;
  dy -= dy2;
  state->transformDelta(dx, dy, &w1, &h1);
  state->transform(xx, yy, &x1, &y1);

  // Throw away characters that are not inside the page boundaries.
  if (x1 + w1 < 0 || x1 > p->pageWidth || y1 + h1 < 0 || y1 > p->pageHeight)
    return;

  // Make a sanity check on character size. Note: (x != x) <-> isnan(x)
  if (x1 != x1 || y1 != y1 || w1 != w1 || h1 != h1)
    return;

  for (int i = 0; i < uLen; i++) {
    // Soft hyphen markers are skipped, as they are invisible unless
    // rendering is done to an actual device and the hyphenation hint
    // used. MCOutputDev extracts the *visible* text content.
    if (u[i] != 0x00AD)
      p->mcOps.push_back(MCOp(u[i]));
  }
}


void MCOutputDev::updateFont(GfxState *state)
{
  GfxFont *font = state->getFont();
  if (!font || font == p->lastFont) return;

  if (!p->lastFont || (p->lastFont->getFamily() && p->lastFont->getFamily()->cmp(font->getFamily()))) {
    if (p->capturing && font->getFamily())
      p->mcOps.push_back(MCOp(font->getFamily()->getCString()));
    if (p->lastFont) p->lastFont->decRefCnt();
    p->lastFont = font;
    font->incRefCnt();
  }

  Guint flags = 0;

  if (font->isFixedWidth()) flags |= MCOp::FlagFontItalic;
  if (font->isItalic()) flags |= MCOp::FlagFontItalic;
  if (font->isBold()) flags |= MCOp::FlagFontBold;
  else {
    switch (font->getWeight()) {
      case GfxFont::W700: // Font weights over 600 are bold
      case GfxFont::W800:
      case GfxFont::W900:
        flags |= MCOp::FlagFontBold;
      default:
        break;
    }
  }

  if (p->lastFlags != flags) {
    if (p->capturing)
      p->mcOps.push_back(MCOp(MCOp::Flags, flags));
    p->lastFlags = flags;
  }
}


const MCOpArray& MCOutputDev::getMCOps() const
{
  return p->mcOps;
}
