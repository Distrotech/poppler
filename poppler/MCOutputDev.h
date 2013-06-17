//========================================================================
//
// MCOutputDev.h
//
// Copyright 2013 Igalia S.L.
//
//========================================================================

#ifndef MCDOUTPUTDEV_H
#define MCDOUTPUTDEV_H

#include "goo/gtypes.h"
#include "goo/gmem.h"
#include "OutputDev.h"
#include <vector>

class GfxState;
class GooString;
class Dict;

struct MCOp {
  enum Type {
    Unichar,
    FontName,
    Flags,
    Color,
  };

  struct OpColor {
    double r, g, b;

    Guint rgbPixel() const {
      return ((Guint) (r * 255) & 0xFF) << 16
           | ((Guint) (g * 255) & 0xFF) << 8
           | ((Guint) (b * 255) & 0xFF);
    }
  };

  enum Flags {
    FlagFontBold   = (1 << 0),
    FlagFontItalic = (1 << 1),
    FlagFontFixed  = (1 << 2),
  };

  Type type;

  union {
    Unicode unichar;
    char   *value;
    Guint   flags;
    OpColor color;
  };

  MCOp(const MCOp &op): type(op.type) {
    switch (type) {
      case Flags: flags = op.flags; break;
      case Unichar: unichar = op.unichar; break;
      case Color: memcpy(&color, &op.color, sizeof(OpColor)); break;
      case FontName: value = strdup(op.value); break;
    }
  }
  ~MCOp() {
    switch (type) {
      case FontName: gfree(value); break;
      default: /* nothing */ break;
    }
  }
  MCOp(): type(FontName), value(NULL) {}
  MCOp(Unicode u): type(Unichar), unichar(u) {}
  MCOp(const char *s): type(FontName), value(strdup(s)) {}
  MCOp(Type t, Guint f = 0): type(t), flags(f) {}
};


typedef std::vector<MCOp> MCOpArray;


class MCOutputDev: public OutputDev {
public:
  MCOutputDev(int mcid);
  virtual ~MCOutputDev();

  virtual GBool isOk() { return gTrue; }
  virtual GBool upsideDown() { return gTrue; }
  virtual GBool useDrawChar() { return gTrue; }
  virtual GBool interpretType3Chars() { return gFalse; }
  virtual GBool needNonText() { return gFalse; }
  virtual GBool needCharCount() { return gFalse; }

  virtual void startPage(int pageNum, GfxState *state, XRef *xref);
  virtual void endPage();

  virtual void restoreState(GfxState *state) { updateFont(state); }
  virtual void updateFont(GfxState *state);

  virtual void drawChar(GfxState *state,
                        double xx, double yy,
                        double dx, double dy,
                        double ox, double oy,
                        CharCode c, int nBytes,
                        Unicode *u, int uLen);

  virtual void beginMarkedContent(char *name, Dict *properties);
  virtual void endMarkedContent(GfxState *state);

  const MCOpArray& getMCOps() const;

private:
  struct Priv;
  Priv *p;
};

#endif /* !MCOUTPUTDEV_H */
