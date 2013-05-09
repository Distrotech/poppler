/* poppler-structure-element.h: glib interface to poppler
 *
 * Copyright (C) 2013 Igalia S.L.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __POPPLER_STRUCTURE_ELEMENT_H__
#define __POPPLER_STRUCTURE_ELEMENT_H__

#include <glib-object.h>
#include "poppler.h"

G_BEGIN_DECLS

#define POPPLER_TYPE_STRUCTURE_ELEMENT    (poppler_structure_element_get_type ())
#define POPPLER_STRUCTURE_ELEMENT(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), POPPLER_TYPE_STRUCTURE_ELEMENT, PopplerStructureElement))
#define POPPLER_IS_STRUCTURE_ELEMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POPPLER_TYPE_STRUCTURE_ELEMENT))

/**
 * PopplerStructureElementKind:
 */
typedef enum {
  POPPLER_STRUCTURE_ELEMENT_UNKNOWN,
  POPPLER_STRUCTURE_ELEMENT_CONTENT,
  POPPLER_STRUCTURE_ELEMENT_OBJECT_REFERENCE,
  POPPLER_STRUCTURE_ELEMENT_DOCUMENT,
  POPPLER_STRUCTURE_ELEMENT_PART,
  POPPLER_STRUCTURE_ELEMENT_ARTICLE,
  POPPLER_STRUCTURE_ELEMENT_SECTION,
  POPPLER_STRUCTURE_ELEMENT_DIV,
  POPPLER_STRUCTURE_ELEMENT_SPAN,
  POPPLER_STRUCTURE_ELEMENT_QUOTE,
  POPPLER_STRUCTURE_ELEMENT_NOTE,
  POPPLER_STRUCTURE_ELEMENT_REFERENCE,
  POPPLER_STRUCTURE_ELEMENT_BIBENTRY,
  POPPLER_STRUCTURE_ELEMENT_CODE,
  POPPLER_STRUCTURE_ELEMENT_LINK,
  POPPLER_STRUCTURE_ELEMENT_ANNOT,
  POPPLER_STRUCTURE_ELEMENT_RUBY,
  POPPLER_STRUCTURE_ELEMENT_WARICHU,
  POPPLER_STRUCTURE_ELEMENT_BLOCKQUOTE,
  POPPLER_STRUCTURE_ELEMENT_CAPTION,
  POPPLER_STRUCTURE_ELEMENT_NONSTRUCT,
  POPPLER_STRUCTURE_ELEMENT_TOC,
  POPPLER_STRUCTURE_ELEMENT_TOC_ITEM,
  POPPLER_STRUCTURE_ELEMENT_INDEX,
  POPPLER_STRUCTURE_ELEMENT_PRIVATE,
  POPPLER_STRUCTURE_ELEMENT_PARAGRAPH,
  POPPLER_STRUCTURE_ELEMENT_HEADING,
  POPPLER_STRUCTURE_ELEMENT_HEADING_1,
  POPPLER_STRUCTURE_ELEMENT_HEADING_2,
  POPPLER_STRUCTURE_ELEMENT_HEADING_3,
  POPPLER_STRUCTURE_ELEMENT_HEADING_4,
  POPPLER_STRUCTURE_ELEMENT_HEADING_5,
  POPPLER_STRUCTURE_ELEMENT_HEADING_6,
  POPPLER_STRUCTURE_ELEMENT_LIST,
  POPPLER_STRUCTURE_ELEMENT_LIST_ITEM,
  POPPLER_STRUCTURE_ELEMENT_LIST_LABEL,
  POPPLER_STRUCTURE_ELEMENT_TABLE,
  POPPLER_STRUCTURE_ELEMENT_TABLE_ROW,
  POPPLER_STRUCTURE_ELEMENT_TABLE_HEADING,
  POPPLER_STRUCTURE_ELEMENT_TABLE_DATA,
  POPPLER_STRUCTURE_ELEMENT_TABLE_HEADER,
  POPPLER_STRUCTURE_ELEMENT_TABLE_FOOTER,
  POPPLER_STRUCTURE_ELEMENT_TABLE_BODY,
  POPPLER_STRUCTURE_ELEMENT_FIGURE,
  POPPLER_STRUCTURE_ELEMENT_FORMULA,
  POPPLER_STRUCTURE_ELEMENT_FORM,
} PopplerStructureElementKind;

/**
 * PopplerStructureAttribute:
 */
typedef enum {
  POPPLER_STRUCTURE_ATTRIBUTE_UNKNOWN,
  POPPLER_STRUCTURE_ATTRIBUTE_USER_PROPERTY,
  POPPLER_STRUCTURE_ATTRIBUTE_PLACEMENT,
  POPPLER_STRUCTURE_ATTRIBUTE_WRITING_MODE,
  POPPLER_STRUCTURE_ATTRIBUTE_BACKGROUND_COLOR,
  POPPLER_STRUCTURE_ATTRIBUTE_BORDER_COLOR,
  POPPLER_STRUCTURE_ATTRIBUTE_BORDER_STYLE,
  POPPLER_STRUCTURE_ATTRIBUTE_BORDER_THICKNESS,
  POPPLER_STRUCTURE_ATTRIBUTE_COLOR,
  POPPLER_STRUCTURE_ATTRIBUTE_PADDING,
  POPPLER_STRUCTURE_ATTRIBUTE_SPACE_BEFORE,
  POPPLER_STRUCTURE_ATTRIBUTE_SPACE_AFTER,
  POPPLER_STRUCTURE_ATTRIBUTE_START_INDENT,
  POPPLER_STRUCTURE_ATTRIBUTE_END_INDENT,
  POPPLER_STRUCTURE_ATTRIBUTE_TEXT_INDENT,
  POPPLER_STRUCTURE_ATTRIBUTE_TEXT_ALIGN,
  POPPLER_STRUCTURE_ATTRIBUTE_BBOX,
  POPPLER_STRUCTURE_ATTRIBUTE_WIDTH,
  POPPLER_STRUCTURE_ATTRIBUTE_HEIGHT,
  POPPLER_STRUCTURE_ATTRIBUTE_BLOCK_ALIGN,
  POPPLER_STRUCTURE_ATTRIBUTE_INLINE_ALIGN,
  POPPLER_STRUCTURE_ATTRIBUTE_TABLE_BORDER_STYLE,
  POPPLER_STRUCTURE_ATTRIBUTE_TABLE_PADDING,
  POPPLER_STRUCTURE_ATTRIBUTE_BASELINE_SHIFT,
  POPPLER_STRUCTURE_ATTRIBUTE_LINE_HEIGHT,
  POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_COLOR,
  POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_THICKNESS,
  POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION,
  POPPLER_STRUCTURE_ATTRIBUTE_RUBY_ALIGN,
  POPPLER_STRUCTURE_ATTRIBUTE_RUBY_POSITION,
  POPPLER_STRUCTURE_ATTRIBUTE_GLYPH_ORIENTATION,
  POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_COUNT,
  POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_GAP,
  POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_WIDTHS,
  POPPLER_STRUCTURE_ATTRIBUTE_LIST_NUMBERING,
  POPPLER_STRUCTURE_ATTRIBUTE_ROLE,
  POPPLER_STRUCTURE_ATTRIBUTE_CHECKED,
  POPPLER_STRUCTURE_ATTRIBUTE_DESCRIPTION,
  POPPLER_STRUCTURE_ATTRIBUTE_ROW_SPAN,
  POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_SPAN,
  POPPLER_STRUCTURE_ATTRIBUTE_HEADERS,
  POPPLER_STRUCTURE_ATTRIBUTE_SCOPE,
  POPPLER_STRUCTURE_ATTRIBUTE_SUMMARY,
} PopplerStructureAttribute;

/**
 * PopplerStructurePlacement:
 */
typedef enum {
  POPPLER_STRUCTURE_PLACEMENT_BLOCK,
  POPPLER_STRUCTURE_PLACEMENT_INLINE,
  POPPLER_STRUCTURE_PLACEMENT_BEFORE,
  POPPLER_STRUCTURE_PLACEMENT_START,
  POPPLER_STRUCTURE_PLACEMENT_END,
} PopplerStructurePlacement;

/**
 * PopplerStructureWritingMode:
 */
typedef enum {
  POPPLER_STRUCTURE_WRITING_MODE_LR_TB,
  POPPLER_STRUCTURE_WRITING_MODE_RL_TB,
  POPPLER_STRUCTURE_WRITING_MODE_TB_RL,
} PopplerStructureWritingMode;

/**
 * PopplerStructureBorderStyle:
 */
typedef enum {
  POPPLER_STRUCTURE_BORDER_STYLE_NONE,
  POPPLER_STRUCTURE_BORDER_STYLE_HIDDEN,
  POPPLER_STRUCTURE_BORDER_STYLE_DOTTED,
  POPPLER_STRUCTURE_BORDER_STYLE_DASHED,
  POPPLER_STRUCTURE_BORDER_STYLE_SOLID,
  POPPLER_STRUCTURE_BORDER_STYLE_DOUBLE,
  POPPLER_STRUCTURE_BORDER_STYLE_GROOVE,
  POPPLER_STRUCTURE_BORDER_STYLE_INSET,
  POPPLER_STRUCTURE_BORDER_STYLE_OUTSET,
} PopplerStructureBorderStyle;

/**
 * PopplerStructureTextAlign:
 */
typedef enum {
  POPPLER_STRUCTURE_TEXT_ALIGN_START,
  POPPLER_STRUCTURE_TEXT_ALIGN_CENTER,
  POPPLER_STRUCTURE_TEXT_ALIGN_END,
  POPPLER_STRUCTURE_TEXT_ALIGN_JUSTIFY,
} PopplerStructureTextAlign;

/**
 * PopplerStructureBlockAlign:
 */
typedef enum {
  POPPLER_STRUCTURE_BLOCK_ALIGN_BEFORE,
  POPPLER_STRUCTURE_BLOCK_ALIGN_MIDDLE,
  POPPLER_STRUCTURE_BLOCK_ALIGN_AFTER,
  POPPLER_STRUCTURE_BLOCK_ALIGN_JUSTIFY,
} PopplerStructureBlockAlign;

/**
 * PopplerStructureInlineAlign:
 */
typedef enum {
  POPPLER_STRUCTURE_INLINE_ALIGN_START,
  POPPLER_STRUCTURE_INLINE_ALIGN_CENTER,
  POPPLER_STRUCTURE_INLINE_ALIGN_END,
} PopplerStructureInlineAlign;

/**
 * PopplerStructureTextDecoration:
 */
typedef enum {
  POPPLER_STRUCTURE_TEXT_DECORATION_NONE,
  POPPLER_STRUCTURE_TEXT_DECORATION_UNDERLINE,
  POPPLER_STRUCTURE_TEXT_DECORATION_OVERLINE,
  POPPLER_STRUCTURE_TEXT_DECORATION_LINETHROUGH,
} PopplerStructureTextDecoration;

/**
 * PopplerStructureRubyAlign:
 */
typedef enum
{
  POPPLER_STRUCTURE_RUBY_ALIGN_START,
  POPPLER_STRUCTURE_RUBY_ALIGN_CENTER,
  POPPLER_STRUCTURE_RUBY_ALIGN_END,
  POPPLER_STRUCTURE_RUBY_ALIGN_JUSTIFY,
  POPPLER_STRUCTURE_RUBY_ALIGN_DISTRIBUTE,
} PopplerStructureRubyAlign;

/**
 * PopplerStructureRubyPosition:
 */
typedef enum {
  POPPLER_STRUCTURE_RUBY_POSITION_BEFORE,
  POPPLER_STRUCTURE_RUBY_POSITION_AFTER,
  POPPLER_STRUCTURE_RUBY_POSITION_WARICHU,
  POPPLER_STRUCTURE_RUBY_POSITION_INLINE,
} PopplerStructureRubyPosition;

/**
 * PopplerStructureGlyphOrientation:
 */
typedef enum {
  POPPLER_STRUCTURE_GLYPH_ORIENTATION_AUTO,
  POPPLER_STRUCTURE_GLYPH_ORIENTATION_0 = POPPLER_STRUCTURE_GLYPH_ORIENTATION_AUTO,
  POPPLER_STRUCTURE_GLYPH_ORIENTATION_90,
  POPPLER_STRUCTURE_GLYPH_ORIENTATION_180,
  POPPLER_STRUCTURE_GLYPH_ORIENTATION_270,
} PopplerStructureGlyphOrientation;

/**
 * PopplerStructureListNumbering:
 */
typedef enum {
  POPPLER_STRUCTURE_LIST_NUMBERING_NONE,
  POPPLER_STRUCTURE_LIST_NUMBERING_DISC,
  POPPLER_STRUCTURE_LIST_NUMBERING_CIRCLE,
  POPPLER_STRUCTURE_LIST_NUMBERING_SQUARE,
  POPPLER_STRUCTURE_LIST_NUMBERING_DECIMAL,
  POPPLER_STRUCTURE_LIST_NUMBERING_UPPER_ROMAN,
  POPPLER_STRUCTURE_LIST_NUMBERING_LOWER_ROMAN,
  POPPLER_STRUCTURE_LIST_NUMBERING_UPPER_ALPHA,
  POPPLER_STRUCTURE_LIST_NUMBERING_LOWER_ALPHA,
} PopplerStructureListNumbering;

/**
 * PopplerStructureRole:
 */
typedef enum {
  POPPLER_STRUCTURE_ROLE_RADIO_BUTTON,
  POPPLER_STRUCTURE_ROLE_PUSH_BUTTON,
  POPPLER_STRUCTURE_ROLE_TEXT_VALUE,
  POPPLER_STRUCTURE_ROLE_CHECKBOX,
} PopplerStructureRole;

/**
 * PopplerStructureChecked:
 */
typedef enum {
  POPPLER_STRUCTURE_CHECKED_ON,
  POPPLER_STRUCTURE_CHECKED_OFF,
  POPPLER_STRUCTURE_CHECKED_NEUTRAL,
} PopplerStructureChecked;

/**
 * PopplerStructureScope:
 */
typedef enum {
  POPPLER_STRUCTURE_SCOPE_ROW,
  POPPLER_STRUCTURE_SCOPE_COLUMN,
  POPPLER_STRUCTURE_SCOPE_BOTH,
} PopplerStructureScope;

/**
 * PopplerStructureReference:
 */
typedef enum {
  POPPLER_STRUCTURE_REFERENCE_UNKNOWN,
  POPPLER_STRUCTURE_REFERENCE_ANNOT,
  POPPLER_STRUCTURE_REFERENCE_LINK,
} PopplerStructureReference;


typedef struct _PopplerTextSpan PopplerTextSpan;
struct _PopplerTextSpan {
  gchar *text;
  gchar *font_name;
  gchar *link_target;
  guint  flags;
  guint  color; /* 0x00RRGGBB */
};

enum {
  POPPLER_TEXT_SPAN_FIXED_WIDTH = (1 << 0),
  POPPLER_TEXT_SPAN_SERIF_FONT  = (1 << 1),
  POPPLER_TEXT_SPAN_ITALIC      = (1 << 2),
  POPPLER_TEXT_SPAN_BOLD        = (1 << 3),
  POPPLER_TEXT_SPAN_LINK        = (1 << 4),
  POPPLER_TEXT_SPAN_COLOR       = (1 << 5),
  POPPLER_TEXT_SPAN_FONT        = (1 << 6),
};


static inline gboolean poppler_text_span_is_fixed_width (PopplerTextSpan *poppler_text_span)
{
  return (poppler_text_span->flags & POPPLER_TEXT_SPAN_FIXED_WIDTH);
}

static inline gboolean poppler_text_span_is_serif_font (PopplerTextSpan *poppler_text_span)
{
  return (poppler_text_span->flags & POPPLER_TEXT_SPAN_SERIF_FONT);
}

static inline gboolean poppler_text_span_is_bold (PopplerTextSpan *poppler_text_span)
{
  return (poppler_text_span->flags & POPPLER_TEXT_SPAN_BOLD);
}

static inline gboolean poppler_text_span_is_link (PopplerTextSpan *poppler_text_span)
{
  return (poppler_text_span->flags & POPPLER_TEXT_SPAN_LINK);
}


GType                       poppler_structure_element_get_type            (void) G_GNUC_CONST;
PopplerStructureElementKind poppler_structure_element_get_kind            (PopplerStructureElement  *poppler_structure_element);
gint                        poppler_structure_element_get_page            (PopplerStructureElement  *poppler_structure_element);
gboolean                    poppler_structure_element_is_content          (PopplerStructureElement  *poppler_structure_element);
gboolean                    poppler_structure_element_is_inline           (PopplerStructureElement  *poppler_structure_element);
gboolean                    poppler_structure_element_is_block            (PopplerStructureElement  *poppler_structure_element);
guint                       poppler_structure_element_get_n_children      (PopplerStructureElement  *poppler_structure_element);
PopplerStructureElement    *poppler_structure_element_get_child           (PopplerStructureElement  *poppler_structure_element,
                                                                           guint                     index);
const gchar                *poppler_structure_element_get_id              (PopplerStructureElement  *poppler_structure_element);
const gchar                *poppler_structure_element_get_title           (PopplerStructureElement  *poppler_structure_element);
const gchar                *poppler_structure_element_get_abbreviation    (PopplerStructureElement  *poppler_structure_element);
const gchar                *poppler_structure_element_get_language        (PopplerStructureElement  *poppler_structure_element);
const gchar                *poppler_structure_element_get_text            (PopplerStructureElement  *poppler_structure_element,
                                                                           gboolean                  recursive);
GList                      *poppler_structure_element_get_text_spans      (PopplerStructureElement  *poppler_structure_element,
                                                                           gboolean                  recursive);
const gchar                *poppler_structure_element_get_alt_text        (PopplerStructureElement  *poppler_structure_element);
const gchar                *poppler_structure_element_get_actual_text     (PopplerStructureElement  *poppler_structure_element);
GVariant                   *poppler_structure_element_get_attribute       (PopplerStructureElement  *poppler_structure_element,
                                                                           PopplerStructureAttribute attribute,
                                                                           gboolean                  inherit);
gboolean                    poppler_structure_element_is_reference        (PopplerStructureElement  *poppler_structure_element);
PopplerStructureReference   poppler_structure_element_get_reference_type  (PopplerStructureElement  *poppler_structure_element);
PopplerLinkMapping         *poppler_structure_element_get_reference_link  (PopplerStructureElement  *poppler_structure_element);
PopplerLinkMapping         *poppler_structure_element_find_link           (PopplerStructureElement  *poppler_structure_element);

PopplerFormField           *poppler_structure_element_get_form_field      (PopplerStructureElement  *poppler_structure_element);
PopplerFormFieldMapping    *poppler_structure_element_get_form_field_mapping (PopplerStructureElement *poppler_structure_element);

G_END_DECLS

#endif /* !__POPPLER_STRUCTURE_ELEMENT_H__ */
