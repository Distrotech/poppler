/* poppler-structure.cc: glib interface to poppler
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

#include "config.h"

#ifndef __GI_SCANNER__
#include <StructTreeRoot.h>
#include <StructElement.h>
#include <GlobalParams.h>
#include <UnicodeMap.h>
#endif /* !__GI_SCANNER__ */

#include "poppler.h"
#include "poppler-private.h"
#include "poppler-structure-element.h"


static inline PopplerStructureElementKind
_poppler_structelement_type_to_poppler_structure_element_kind (StructElement::Type type)
{
  switch (type)
    {
      case StructElement::Unknown:
        return POPPLER_STRUCTURE_ELEMENT_UNKNOWN;
      case StructElement::MCID:
        return POPPLER_STRUCTURE_ELEMENT_CONTENT;
      case StructElement::OBJR:
        return POPPLER_STRUCTURE_ELEMENT_OBJECT_REFERENCE;
      case StructElement::Document:
        return POPPLER_STRUCTURE_ELEMENT_DOCUMENT;
      case StructElement::Part:
        return POPPLER_STRUCTURE_ELEMENT_PART;
      case StructElement::Sect:
        return POPPLER_STRUCTURE_ELEMENT_SECTION;
      case StructElement::Div:
        return POPPLER_STRUCTURE_ELEMENT_DIV;
      case StructElement::Span:
        return POPPLER_STRUCTURE_ELEMENT_SPAN;
      case StructElement::Quote:
        return POPPLER_STRUCTURE_ELEMENT_QUOTE;
      case StructElement::Note:
        return POPPLER_STRUCTURE_ELEMENT_NOTE;
      case StructElement::Reference:
        return POPPLER_STRUCTURE_ELEMENT_REFERENCE;
      case StructElement::BibEntry:
        return POPPLER_STRUCTURE_ELEMENT_BIBENTRY;
      case StructElement::Code:
        return POPPLER_STRUCTURE_ELEMENT_CODE;
      case StructElement::Link:
        return POPPLER_STRUCTURE_ELEMENT_LINK;
      case StructElement::Annot:
        return POPPLER_STRUCTURE_ELEMENT_ANNOT;
      case StructElement::Ruby:
        return POPPLER_STRUCTURE_ELEMENT_RUBY;
      case StructElement::Warichu:
        return POPPLER_STRUCTURE_ELEMENT_WARICHU;
      case StructElement::BlockQuote:
        return POPPLER_STRUCTURE_ELEMENT_BLOCKQUOTE;
      case StructElement::Caption:
        return POPPLER_STRUCTURE_ELEMENT_CAPTION;
      case StructElement::NonStruct:
        return POPPLER_STRUCTURE_ELEMENT_NONSTRUCT;
      case StructElement::TOC:
        return POPPLER_STRUCTURE_ELEMENT_TOC;
      case StructElement::TOCI:
        return POPPLER_STRUCTURE_ELEMENT_TOC_ITEM;
      case StructElement::Index:
        return POPPLER_STRUCTURE_ELEMENT_INDEX;
      case StructElement::Private:
        return POPPLER_STRUCTURE_ELEMENT_PRIVATE;
      case StructElement::P:
        return POPPLER_STRUCTURE_ELEMENT_PARAGRAPH;
      case StructElement::H:
        return POPPLER_STRUCTURE_ELEMENT_HEADING;
      case StructElement::H1:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_1;
      case StructElement::H2:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_2;
      case StructElement::H3:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_3;
      case StructElement::H4:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_4;
      case StructElement::H5:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_5;
      case StructElement::H6:
        return POPPLER_STRUCTURE_ELEMENT_HEADING_6;
      case StructElement::L:
        return POPPLER_STRUCTURE_ELEMENT_LIST;
      case StructElement::LI:
        return POPPLER_STRUCTURE_ELEMENT_LIST_ITEM;
      case StructElement::Lbl:
        return POPPLER_STRUCTURE_ELEMENT_LIST_LABEL;
      case StructElement::Table:
        return POPPLER_STRUCTURE_ELEMENT_TABLE;
      case StructElement::TR:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_ROW;
      case StructElement::TH:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_HEADING;
      case StructElement::TD:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_DATA;
      case StructElement::THead:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_HEADER;
      case StructElement::TFoot:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_FOOTER;
      case StructElement::TBody:
        return POPPLER_STRUCTURE_ELEMENT_TABLE_BODY;
      case StructElement::Figure:
        return POPPLER_STRUCTURE_ELEMENT_FIGURE;
      case StructElement::Formula:
        return POPPLER_STRUCTURE_ELEMENT_FORMULA;
      case StructElement::Form:
        return POPPLER_STRUCTURE_ELEMENT_FORM;
      default:
        g_assert_not_reached ();
    }
}

static GBool
_rgb_array_to_doubles(Array *array, double rgb[3])
{
  double r, g, b;
  Object obj;

  if (array->getLength() != 3)
    return FALSE;

  if (!array->getNF(0, &obj)->isReal()) goto not_a_real;
  r = obj.getReal();
  obj.free();

  if (!array->getNF(1, &obj)->isReal()) goto not_a_real;
  g = obj.getReal();
  obj.free();

  if (!array->getNF(2, &obj)->isReal()) goto not_a_real;
  b = obj.getReal();
  obj.free();

  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;

  return TRUE;

not_a_real:
  obj.free();
  return FALSE;
}


static GVariant*
_g_variant_new_from_rgb_array (Array *array)
{
  double v[3];

  g_return_val_if_fail (array->getLength () == 3, NULL);

  if (!_rgb_array_to_doubles (array, v))
    return NULL;

  return g_variant_new ("(ddd)", v[0], v[1], v[2]);
}


static GVariant*
_g_variant_new_from_rgb_array_or_x4 (Array *array)
{
  double v[12];

  if (array->getLength() == 3)
    {
      if (!_rgb_array_to_doubles (array, v))
        return NULL;

      v[ 9] = v[6] = v[3] = v[0];
      v[10] = v[7] = v[4] = v[1];
      v[11] = v[8] = v[5] = v[2];
    }
  else if (array->getLength () == 4)
    {
      for (int i = 0; i < 4; i++)
        {
          Object item;
          if (!array->get(i, &item)->isArray())
            return NULL;
          if (!_rgb_array_to_doubles (item.getArray(), &v[i * 3]))
            return NULL;
        }
    }
  else
    return NULL;

  return g_variant_new ("((ddd)(ddd)(ddd)(ddd))",
                        v[ 0], v[ 1], v[ 2],
                        v[ 3], v[ 4], v[ 5],
                        v[ 6], v[ 7], v[ 8],
                        v[ 9], v[10], v[11]);
}


template <typename EnumType>
struct EnumNameValue {
  const gchar *name;
  EnumType     value;

  static const EnumNameValue<EnumType> values[];
  static const EnumType null = static_cast<EnumType> (-1);
};

template<>
const EnumNameValue<PopplerStructurePlacement> EnumNameValue<PopplerStructurePlacement>::values[] =
{
  { "Block",  POPPLER_STRUCTURE_PLACEMENT_BLOCK  },
  { "Inline", POPPLER_STRUCTURE_PLACEMENT_INLINE },
  { "Before", POPPLER_STRUCTURE_PLACEMENT_BEFORE },
  { "Start",  POPPLER_STRUCTURE_PLACEMENT_START  },
  { "End",    POPPLER_STRUCTURE_PLACEMENT_END    },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureWritingMode> EnumNameValue<PopplerStructureWritingMode>::values[] =
{
  { "LrTb", POPPLER_STRUCTURE_WRITING_MODE_LR_TB },
  { "RlTb", POPPLER_STRUCTURE_WRITING_MODE_RL_TB },
  { "TbRl", POPPLER_STRUCTURE_WRITING_MODE_TB_RL },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureBorderStyle> EnumNameValue<PopplerStructureBorderStyle>::values[] =
{
  { "None",   POPPLER_STRUCTURE_BORDER_STYLE_NONE   },
  { "Hidden", POPPLER_STRUCTURE_BORDER_STYLE_HIDDEN },
  { "Dotted", POPPLER_STRUCTURE_BORDER_STYLE_DOTTED },
  { "Dashed", POPPLER_STRUCTURE_BORDER_STYLE_DASHED },
  { "Solid",  POPPLER_STRUCTURE_BORDER_STYLE_SOLID  },
  { "Double", POPPLER_STRUCTURE_BORDER_STYLE_DOUBLE },
  { "Groove", POPPLER_STRUCTURE_BORDER_STYLE_GROOVE },
  { "Inset",  POPPLER_STRUCTURE_BORDER_STYLE_INSET  },
  { "Outset", POPPLER_STRUCTURE_BORDER_STYLE_OUTSET },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureTextAlign> EnumNameValue<PopplerStructureTextAlign>::values[] =
{
  { "Start",   POPPLER_STRUCTURE_TEXT_ALIGN_START   },
  { "Center",  POPPLER_STRUCTURE_TEXT_ALIGN_CENTER  },
  { "End",     POPPLER_STRUCTURE_TEXT_ALIGN_END     },
  { "Justify", POPPLER_STRUCTURE_TEXT_ALIGN_JUSTIFY },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureBlockAlign> EnumNameValue<PopplerStructureBlockAlign>::values[] =
{
  { "Before",  POPPLER_STRUCTURE_BLOCK_ALIGN_BEFORE  },
  { "Middle",  POPPLER_STRUCTURE_BLOCK_ALIGN_MIDDLE  },
  { "After",   POPPLER_STRUCTURE_BLOCK_ALIGN_AFTER   },
  { "Justify", POPPLER_STRUCTURE_BLOCK_ALIGN_JUSTIFY },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureInlineAlign> EnumNameValue<PopplerStructureInlineAlign>::values[] =
{
  { "Start",  POPPLER_STRUCTURE_INLINE_ALIGN_START  },
  { "Center", POPPLER_STRUCTURE_INLINE_ALIGN_CENTER },
  { "End",    POPPLER_STRUCTURE_INLINE_ALIGN_END    },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureTextDecoration> EnumNameValue<PopplerStructureTextDecoration>::values[] =
{
  { "None",        POPPLER_STRUCTURE_TEXT_DECORATION_NONE        },
  { "Underline",   POPPLER_STRUCTURE_TEXT_DECORATION_UNDERLINE   },
  { "Overline",    POPPLER_STRUCTURE_TEXT_DECORATION_OVERLINE    },
  { "LineThrough", POPPLER_STRUCTURE_TEXT_DECORATION_LINETHROUGH },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureRubyAlign> EnumNameValue<PopplerStructureRubyAlign>::values[] =
{
  { "Start",      POPPLER_STRUCTURE_RUBY_ALIGN_START      },
  { "Center",     POPPLER_STRUCTURE_RUBY_ALIGN_CENTER     },
  { "End",        POPPLER_STRUCTURE_RUBY_ALIGN_END        },
  { "Justify",    POPPLER_STRUCTURE_RUBY_ALIGN_JUSTIFY    },
  { "Distribute", POPPLER_STRUCTURE_RUBY_ALIGN_DISTRIBUTE },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureRubyPosition> EnumNameValue<PopplerStructureRubyPosition>::values[] =
{
  { "Before",  POPPLER_STRUCTURE_RUBY_POSITION_BEFORE  },
  { "After",   POPPLER_STRUCTURE_RUBY_POSITION_AFTER   },
  { "Warichu", POPPLER_STRUCTURE_RUBY_POSITION_WARICHU },
  { "Inline",  POPPLER_STRUCTURE_RUBY_POSITION_INLINE  },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureGlyphOrientation> EnumNameValue<PopplerStructureGlyphOrientation>::values[] =
{
  { "Auto", POPPLER_STRUCTURE_GLYPH_ORIENTATION_AUTO },
  { "90",   POPPLER_STRUCTURE_GLYPH_ORIENTATION_90   },
  { "180",  POPPLER_STRUCTURE_GLYPH_ORIENTATION_180  },
  { "270",  POPPLER_STRUCTURE_GLYPH_ORIENTATION_270  },
  { "360",  POPPLER_STRUCTURE_GLYPH_ORIENTATION_0    },
  { "-90",  POPPLER_STRUCTURE_GLYPH_ORIENTATION_270  },
  { "-180", POPPLER_STRUCTURE_GLYPH_ORIENTATION_180  },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureListNumbering> EnumNameValue<PopplerStructureListNumbering>::values[] =
{
  { "None",       POPPLER_STRUCTURE_LIST_NUMBERING_NONE        },
  { "Disc",       POPPLER_STRUCTURE_LIST_NUMBERING_DISC        },
  { "Circle",     POPPLER_STRUCTURE_LIST_NUMBERING_CIRCLE      },
  { "Square",     POPPLER_STRUCTURE_LIST_NUMBERING_SQUARE      },
  { "Decimal",    POPPLER_STRUCTURE_LIST_NUMBERING_DECIMAL     },
  { "UpperRoman", POPPLER_STRUCTURE_LIST_NUMBERING_UPPER_ROMAN },
  { "LowerRoman", POPPLER_STRUCTURE_LIST_NUMBERING_LOWER_ROMAN },
  { "UpperAlpha", POPPLER_STRUCTURE_LIST_NUMBERING_UPPER_ALPHA },
  { "LowerAlpha", POPPLER_STRUCTURE_LIST_NUMBERING_LOWER_ALPHA },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureRole> EnumNameValue<PopplerStructureRole>::values[] =
{
  { "rb", POPPLER_STRUCTURE_ROLE_RADIO_BUTTON },
  { "cb", POPPLER_STRUCTURE_ROLE_CHECKBOX     },
  { "pb", POPPLER_STRUCTURE_ROLE_PUSH_BUTTON  },
  { "tv", POPPLER_STRUCTURE_ROLE_TEXT_VALUE   },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureChecked> EnumNameValue<PopplerStructureChecked>::values[] =
{
  { "on",      POPPLER_STRUCTURE_CHECKED_ON      },
  { "off",     POPPLER_STRUCTURE_CHECKED_OFF     },
  { "neutral", POPPLER_STRUCTURE_CHECKED_NEUTRAL },
  { NULL }
};

template<>
const EnumNameValue<PopplerStructureScope> EnumNameValue<PopplerStructureScope>::values[] =
{
  { "Row",    POPPLER_STRUCTURE_SCOPE_ROW    },
  { "Column", POPPLER_STRUCTURE_SCOPE_COLUMN },
  { "Both",   POPPLER_STRUCTURE_SCOPE_BOTH   },
  { NULL }
};

template <typename EnumType>
static EnumType
name_to_enum (Object   *name_value,
              EnumType  default_value = EnumType::null)
{
  if (!name_value)
    return default_value;

  for (const EnumNameValue<EnumType> *item = EnumNameValue<EnumType>::values ; item->name; item++)
    if (name_value->isName (item->name))
      return item->value;

  return default_value;
}


template <typename EnumType>
static GVariant*
name_to_variant_enum (Object  *name_value,
                      EnumType default_value = EnumNameValue<EnumType>::null)
{
  EnumType value = name_to_enum<EnumType> (name_value, default_value);
  return value == EnumNameValue<EnumType>::null ? NULL : g_variant_new_uint32 (value);
}


static GVariant*
string_to_variant (Object *object)
{
  if (object->isName ())
    return g_variant_new_string (object->getName ());
  if (object->isString ())
    {
      gchar *utf8_string = _poppler_goo_string_to_utf8 (object->getString ());
      GVariant* result = g_variant_new_string (utf8_string);
      g_free (utf8_string);
      return result;
    }
  return NULL;
}


static GVariant*
_g_variant_new_from_border_style (Object *object)
{
  PopplerStructureBorderStyle border_style[4];

  if (object->isArray () && object->arrayGetLength () == 4)
    {
      Object item;
      for (int i = 0; i < 4; i++)
        border_style[i] = name_to_enum<PopplerStructureBorderStyle> (object->arrayGet (i, &item),
                                                                     POPPLER_STRUCTURE_BORDER_STYLE_NONE);
    }
  else if (object->isName ())
    {
      border_style[0] = border_style[1] = border_style[2] = border_style[3] =
        name_to_enum <PopplerStructureBorderStyle> (object, POPPLER_STRUCTURE_BORDER_STYLE_NONE);
    }
  else
    return NULL;

  return g_variant_new ("(uuuu)",
                        border_style[0],
                        border_style[1],
                        border_style[2],
                        border_style[3]);
}


static GVariant*
_g_variant_new_from_number_or_x4 (Object *object)
{
  double v[4];

  if (object->isArray () && object->arrayGetLength () == 4)
    {
      Object item;
      for (int i = 0; i < 4; i++)
        {
          if (object->arrayGet (i, &item)->isReal ())
            v[i] = item.getReal ();
          else if (item.isInt ())
            v[i] = (double) item.getInt ();
          else
            return NULL;
        }
    }
  else if (object->isReal ())
    v[0] = v[1] = v[2] = v[3] = object->getReal ();
  else if (object->isInt ())
    v[0] = v[1] = v[2] = v[3] = (double) object->getInt ();
  else
    return NULL;

  return g_variant_new ("(dddd)", v[0], v[1], v[2], v[3]);
}


static inline GVariant*
_g_variant_new_from_number_x4 (Object *object)
{
  return object->isArray () ? _g_variant_new_from_number_or_x4 (object) : NULL;
}


static GVariant*
_g_variant_new_from_number (Object *object)
{
  if (object->isReal ())
    return g_variant_new_double (object->getReal ());
  if (object->isInt ())
    return g_variant_new_double ((double) object->getInt ());
  return NULL;
}


static GVariant*
_g_variant_new_from_number_or_auto (Object *object)
{
  if (object->isName ("Auto"))
    return g_variant_new ("md", NULL);
  if (object->isReal ())
    return g_variant_new ("md", object->getReal ());
  if (object->isInt ())
    return g_variant_new ("md", (double) object->getInt ());
  return NULL;
}


static inline GVariant*
_g_variant_new_from_number_or_auto_or_normal (Object *object)
{
  return object->isName ("Normal") ? g_variant_new ("md", NULL)
                                   : _g_variant_new_from_number_or_auto (object);
}


static GVariant*
_g_variant_new_number_array (Object *object)
{
  GVariantBuilder *builder = g_variant_builder_new (G_VARIANT_TYPE ("ad"));

  if (object->isReal ())
    g_variant_builder_add (builder, "d", object->getReal ());
  else if (object->isInt ())
    g_variant_builder_add (builder, "d", (double) object->getInt ());
  else if (object->isArray ())
    {
      for (int i = 0; i < object->arrayGetLength (); i++)
        {
          Object item;
          if (object->arrayGet (i, &item)->isReal ())
            g_variant_builder_add (builder, "d", item.getReal ());
          else if (item.isInt ())
            g_variant_builder_add (builder, "d", (double) item.getInt ());
        }
    }

  GVariant *value = g_variant_new ("ad", builder);
  g_variant_builder_unref (builder);
  return value;
}


static GVariant*
_g_variant_new_string_array (Object *object)
{
  GVariantBuilder *builder = g_variant_builder_new (G_VARIANT_TYPE ("as"));

  if (object->isName ())
    g_variant_builder_add (builder, "s", object->getName ());
  else if (object->isString ())
    {
      gchar *utf8_string = _poppler_goo_string_to_utf8 (object->getString ());
      g_variant_builder_add (builder, "s", utf8_string);
      g_free (utf8_string);
    }
  else if (object->isArray ())
    {
      for (int i = 0; i < object->arrayGetLength (); i++)
        {
          Object item;
          if (object->arrayGet (i, &item)->isName ())
            g_variant_builder_add (builder, "s", object->getName ());
          else if (object->isString ())
            {
              gchar *utf8_string = _poppler_goo_string_to_utf8 (object->getString ());
              g_variant_builder_add (builder, "s", utf8_string);
              g_free (utf8_string);
            }
        }
    }

  GVariant *value = g_variant_new ("as", builder);
  g_variant_builder_unref (builder);
  return value;
}


static inline Object*
attr_value_or_default (PopplerStructureElement *poppler_structure_element,
                       Attribute::Type          attribute_type,
                       gboolean                 inherit)
{
  Object *value = Attribute::getDefaultValue (attribute_type);
  const Attribute *attr;

  if ((attr = poppler_structure_element->elem->findAttribute (attribute_type, inherit)))
    value = attr->getValue ();

  return value;
}


static void _poppler_text_span_free (gpointer data)
{
  PopplerTextSpan *span = (PopplerTextSpan*) data;
  g_free (span->text);
  g_free (span->font_name);
  g_free (span->link_target);
  g_slice_free (PopplerTextSpan, data);
}


/**
 * SECTION:poppler-structure-element
 * @short_description: Document structure element.
 * @title: PopplerStructureElement
 * @see_also: #PopplerStructure
 *
 * Instances of #PopplerStructureElement are used to describe the structure
 * of a #PopplerDocument. To access the elements in the structure of the
 * document, first use poppler_document_get_structure() to obtain its
 * #PopplerStructure, and then use poppler_structure_get_n_children()
 * and poppler_structure_get_child() to enumerate the top level elements.
 */

typedef struct _PopplerStructureElementClass PopplerStructureElementClass;
struct _PopplerStructureElementClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (PopplerStructureElement, poppler_structure_element, G_TYPE_OBJECT);


PopplerStructureElement*
_poppler_structure_element_new (PopplerStructure *structure, StructElement *element)
{
  PopplerStructureElement *poppler_structure_element;

  g_assert (structure);
  g_assert (element);

  poppler_structure_element = (PopplerStructureElement *) g_object_new (POPPLER_TYPE_STRUCTURE_ELEMENT, NULL, NULL);
  poppler_structure_element->text      = NULL;
  poppler_structure_element->text_r    = NULL;
  poppler_structure_element->children  = NULL;
  poppler_structure_element->structure = structure;
  poppler_structure_element->elem      = element;

  if (element->getNumElements ())
    poppler_structure_element->children = (PopplerStructureElement**) g_new0 (PopplerStructureElement*,
                                                                              element->getNumElements ());
  return poppler_structure_element;
}


static void
poppler_structure_element_init (PopplerStructureElement *poppler_structure_element)
{
}


static void
poppler_structure_element_finalize (GObject *object)
{
  PopplerStructureElement *poppler_structure_element = POPPLER_STRUCTURE_ELEMENT (object);

  /* poppler_structure_element->elem is owned by the StructTreeRoot */
  g_free (poppler_structure_element->language);
  g_free (poppler_structure_element->text_r);
  g_free (poppler_structure_element->text);
  g_free (poppler_structure_element->title);
  g_free (poppler_structure_element->id);
  g_list_free_full (poppler_structure_element->text_spans, _poppler_text_span_free);

  if (poppler_structure_element->children)
    {
      for (unsigned i = 0; i < poppler_structure_element->elem->getNumElements (); i++)
        g_object_unref (poppler_structure_element->children[i]);
      g_free (poppler_structure_element->children);
    }

  G_OBJECT_CLASS (poppler_structure_element_parent_class)->finalize (object);
}


static void
poppler_structure_element_class_init (PopplerStructureElementClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = poppler_structure_element_finalize;
}


/**
 * poppler_structure_element_get_kind:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: A #PopplerStructureElementKind value.
 */
PopplerStructureElementKind
poppler_structure_element_get_kind (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), POPPLER_STRUCTURE_ELEMENT_UNKNOWN);
  g_assert (poppler_structure_element->elem);

  return _poppler_structelement_type_to_poppler_structure_element_kind (poppler_structure_element->elem->getType ());
}

/**
 * poppler_structure_element_get_page:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: Number of the page that contains the element, of
 *    <code>-1</code> if not defined.
 */
gint
poppler_structure_element_get_page (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), POPPLER_STRUCTURE_ELEMENT_UNKNOWN);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->hasPageRef ())
    {
      const Ref ref = poppler_structure_element->elem->getPageRef ();
      return poppler_structure_element->structure->document->doc->findPage(ref.num, ref.gen) - 1;
    }

  return -1;
}

/**
 * poppler_structure_element_is_content:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Checks whether an element is actual document content.
 *
 * Return value: Whether the element is content.
 */
gboolean
poppler_structure_element_is_content (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  g_assert (poppler_structure_element->elem);

  return poppler_structure_element->elem->isContent ();
}

/**
 * poppler_structure_element_is_inline:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Checks whether an element is an inline element.
 *
 * Return value: Whether the element is inline.
 */
gboolean
poppler_structure_element_is_inline (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  g_assert (poppler_structure_element->elem);

  return poppler_structure_element->elem->isInline ();
}

/**
 * poppler_structure_element_is_block:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Checks whether an element is a block element.
 *
 * Return value: Whether the element is block.
 */
gboolean
poppler_structure_element_is_block (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  g_assert (poppler_structure_element->elem);

  return poppler_structure_element->elem->isBlock ();
}


static guint
data_table_score (const StructElement *elem, gboolean *has_th)
{
  g_assert (elem);
  g_assert (has_th);

  guint score = 0;
  for (unsigned i = 0; i < elem->getNumElements (); i++)
    score += data_table_score (elem->getElement (i), has_th);

  switch (elem->getType ())
    {
      case StructElement::THead: score++; break;
      case StructElement::TBody: score++; break;
      case StructElement::TH: *has_th = TRUE; break;
      default: break;
    }

  return score;
}

/**
 * poppler_structure_element_is_data_table:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Note that there is no proper metadata in PDF documents which identify
 * data tables, so heuristics are used to determine whether a table is
 * <em>likely</em> to contain data.
 *
 * Return value: Whether an element is a %POPPLER_STRUCTURE_ELEMENT_TABLE
 *    and the table contains series of data.
 */
gboolean
poppler_structure_element_is_data_table (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->getType () != StructElement::Table)
    return FALSE;

  /*
   * Data tables are likely to have table-header cells, and at least have
   * the contents divided in THead and/or TBody elements. The scoring
   * function counts the later and also sets has_th to TRUE. If the score
   * is more than zero and there is header cells, assume there is a data
   * table.
   */
  gboolean has_th = FALSE;
  return data_table_score (poppler_structure_element->elem, &has_th) && has_th;
}

/**
 * poppler_structure_element_is_layout_table:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Note that there is no proper metadata in PDF documents which identify
 * layout tables, so heuristics are used to determine whether a table is
 * <em>likely</em> to be used for layout purposes.
 *
 * Return value: Whether an element is a %POPPLER_STRUCTURE_ELEMENT_TABLE
 *    and the table is used as aid for layout of page elements.
 */
gboolean
poppler_structure_element_is_layout_table (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  return !poppler_structure_element_is_data_table (poppler_structure_element);
}

/**
 * poppler_structure_element_get_n_children:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Gets the number of children of @structure_element.
 *
 * Return value: Number of children elements.
 */
guint
poppler_structure_element_get_n_children (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), 0);
  g_assert (poppler_structure_element->elem);

  return poppler_structure_element->elem->getNumElements ();
}

/**
 * poppler_structure_element_get_child:
 * @poppler_structure_element: A #PopplerStructureElement
 * @index: Index of the children element to obtain.
 *
 * Return value: (transfer none): A #PopplerStructureElement.
 */
PopplerStructureElement*
poppler_structure_element_get_child (PopplerStructureElement *poppler_structure_element,
                                     guint                    index)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);
  g_assert (poppler_structure_element->elem->getNumElements () >= 0);
  g_return_val_if_fail (index < (guint) poppler_structure_element->elem->getNumElements (), NULL);

  if (!poppler_structure_element->children[index])
    {
      poppler_structure_element->children[index] = _poppler_structure_element_new (poppler_structure_element->structure,
                                                                                   poppler_structure_element->elem->getElement (index));
      g_object_ref_sink (poppler_structure_element->children[index]);
    }
  return poppler_structure_element->children[index];
}

/**
 * poppler_structure_element_get_id:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer none): The identifier of the element (if
 *    defined), or %NULL.
 */
const gchar*
poppler_structure_element_get_id (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->id && poppler_structure_element->elem->getID ())
    poppler_structure_element->id = _poppler_goo_string_to_utf8 (poppler_structure_element->elem->getID ());

  return poppler_structure_element->id;
}

/**
 * poppler_structure_element_get_title:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer none): The title of the element (if defined),
 *    or %NULL.
 */
const gchar*
poppler_structure_element_get_title (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->title && poppler_structure_element->elem->getTitle ())
    poppler_structure_element->title = _poppler_goo_string_to_utf8 (poppler_structure_element->elem->getTitle ());

  return poppler_structure_element->title;
}

/**
 * popppler_structure_element_get_abbreviation:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Acronyms and abbreviations contained in elements of type
 * #POPPLER_STRUCTURE_ELEMENT_SPAN may have an associated expanded
 * text form, which can be retrieved using this function.
 *
 * Return value: (transfer none): Text of the expanded abbreviation, if the
 *    element text is an abbreviation or acronym.
 */
const gchar*
poppler_structure_element_get_abbreviation (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->getType () != StructElement::Span)
    return NULL;

  if (!poppler_structure_element->text_abbrev && poppler_structure_element->elem->getExpandedAbbr ())
    poppler_structure_element->text_abbrev = _poppler_goo_string_to_utf8 (poppler_structure_element->elem->getExpandedAbbr ());

  return poppler_structure_element->text_abbrev;
}

/**
 * poppler_structure_element_get_language:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer none): language and country code, in two-letter
 *    ISO format, e.g. <code>en_US</code>, or %NULL if not defined.
 */
const gchar*
poppler_structure_element_get_language (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->language && poppler_structure_element->elem->getLanguage ())
    poppler_structure_element->language = _poppler_goo_string_to_utf8 (poppler_structure_element->elem->getLanguage ());

  return poppler_structure_element->language;
}

/**
 * poppler_structure_element_get_alt_text:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Obtains the “alternate” text representation of the element (and its child
 * elements). This is mostly used for non-text elements like images and
 * figures, to specify a textual description of the element.
 *
 * Note that for elements containing proper text, the function
 * poppler_structure_element_get_text() must be used instead.
 *
 * Return value: (transfer none): The alternate text representation for the
 *    element, or %NULL if not defined.
 */
const gchar*
poppler_structure_element_get_alt_text (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->alt_text && poppler_structure_element->elem->getAltText ())
    {
      GooString *s = poppler_structure_element->elem->getAltText ();
      if (s)
        poppler_structure_element->alt_text = _poppler_goo_string_to_utf8 (s);
      delete s;
    }

  return poppler_structure_element->alt_text;
}

/**
 * poppler_structure_element_get_actual_text:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Obtains the actual text enclosed by the element (and its child elements).
 * The actual text is mostly used for non-text elements like images and
 * figures which <em>do</em> have the graphical appearance of text, like
 * a logo. For those the actual text is the equivalent text to those
 * graphical elements which look like text when rendered.
 *
 * Note that for elements containing proper text, the function
 * poppler_structure_element_get_text() must be used instead.
 *
 * Return value: (transfer none): The actual text for the element, or %NULL
 *    if not defined.
 */
const gchar*
poppler_structure_element_get_actual_text (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->actual_text && poppler_structure_element->elem->getActualText ())
    {
      GooString *s = poppler_structure_element->elem->getActualText ();
      if (s)
        poppler_structure_element->actual_text = _poppler_goo_string_to_utf8 (s);
      delete s;
    }

  return poppler_structure_element->actual_text;
}

/**
 * poppler_structure_element_get_text:
 * @poppler_structure_element: A #PopplerStructureElement
 * @recursive: If %TRUE, the text of child elements is gathered recursively
 *   in logical order and returned as part of the result.
 *
 * Obtains the text enclosed by an element, or the subtree under an element.
 *
 * Return value: (transfer none): A string.
 */
const gchar*
poppler_structure_element_get_text (PopplerStructureElement *poppler_structure_element,
                                    gboolean                 recursive)
{
  if (recursive)
    {
      if (!poppler_structure_element->text_r)
        {
          GooString *s = poppler_structure_element->elem->getText (NULL, gTrue);
          if (s)
            poppler_structure_element->text_r = _poppler_goo_string_to_utf8 (s);
          delete s;
        }
      return poppler_structure_element->text_r;
    }

  if (!poppler_structure_element->text)
    {
      GooString *s = poppler_structure_element->elem->getText (NULL, gFalse);
      if (s)
        poppler_structure_element->text = _poppler_goo_string_to_utf8 (s);
      delete s;
    }
  return poppler_structure_element->text;
}


class SpanBuilder {
public:
  SpanBuilder():
    font(), text(), link(),
    map(globalParams->getTextEncoding()),
    glist(NULL),
    flags(0),
    color(0)
  {}

  ~SpanBuilder() {
    map->decRefCnt();
    g_list_free_full (glist, _poppler_text_span_free);
  }

  void process(const MCOpArray& ops) {
    for (MCOpArray::const_iterator i = ops.begin(); i != ops.end(); ++i)
      process(*i);
  }

  void process(const MCOp& op) {
    if (op.type == MCOp::Unichar) {
      int n = map->mapUnicode(op.unichar, buf, sizeof(buf));
      text.append(buf, n);
      return;
    }

    Guint oldFlags = flags;

    if (op.type == MCOp::Flags) {
      if (op.flags & MCOp::FlagFontBold)
        flags |= POPPLER_TEXT_SPAN_BOLD;
      else
        flags &= ~POPPLER_TEXT_SPAN_BOLD;

      if (op.flags & MCOp::FlagFontFixed)
        flags |= POPPLER_TEXT_SPAN_FIXED_WIDTH;
      else
        flags &= ~POPPLER_TEXT_SPAN_FIXED_WIDTH;

      if (op.flags & MCOp::FlagFontItalic)
        flags |= POPPLER_TEXT_SPAN_ITALIC;
      else
        flags &= ~POPPLER_TEXT_SPAN_ITALIC;
    }

    if (op.type == MCOp::Color && (color = op.color.rgbPixel ())) {
      flags |= POPPLER_TEXT_SPAN_COLOR;
    } else {
      flags &= ~POPPLER_TEXT_SPAN_COLOR;
    }

    if (op.type == MCOp::FontName) {
      if (op.value) {
        flags |= POPPLER_TEXT_SPAN_FONT;
        font.append(op.value);
      } else {
        flags &= ~POPPLER_TEXT_SPAN_FONT;
      }
    }

    if (flags != oldFlags)
      newSpan();
  }

  void newSpan() {
    // If there is no text, do not append a new PopplerTextSpan
    // and keep the attributes/flags for the next span.
    if (text.getLength ()) {
      PopplerTextSpan *span = g_slice_new0 (PopplerTextSpan);
      span->color = color;
      span->flags = flags;
      span->text = _poppler_goo_string_to_utf8 (&text);
      text.clear();

      if (font.getLength()) {
        span->font_name = _poppler_goo_string_to_utf8 (&font);
        font.clear();
      }

      if (link.getLength()) {
        assert(flags & POPPLER_TEXT_SPAN_LINK);
        span->link_target = _poppler_goo_string_to_utf8 (&link);
      }

      glist = g_list_append (glist, span);
    }

    // Link is always cleared
    link.clear();
  }

  GList* end() {
    GList *result = glist;
    glist = NULL;
    return result;
  }

private:
  GooString font;
  GooString text;
  GooString link;
  UnicodeMap *map;
  GList *glist;
  char buf[8];
  Guint flags;
  Guint color;
};


/**
 * poppler_structure_element_get_text_spans:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Obtains the text enclosed by an element, as a #GList of #PopplerTextSpan
 * structures. Each item in the list is a piece of text which share the same
 * attributes, plus its attributes.
 *
 * Return value: (transfer none) (element-type PopplerTextSpan): A #GList
 *    of #PopplerTextSpan structures.
 */
GList*
poppler_structure_element_get_text_spans (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (!poppler_structure_element->elem->isContent ())
    return NULL;

  if (!poppler_structure_element->text_spans)
    {
      SpanBuilder builder;
      builder.process(poppler_structure_element->elem->getMCOps ());
      poppler_structure_element->text_spans = builder.end();
    }
  return poppler_structure_element->text_spans;
}

/**
 * poppler_structure_element_get_attribute:
 * @poppler_structure_element: A #PopplerStructureElement.
 * @attribute: A #PopperStructureAttribute value.
 * @value (out): A #GValue in which to return the value of the attribute.
 * @inherit: Whether to look up for inheritable attribute values in the
 *    ancestors of the element, if the attribute is not defined in the
 *    element.
 *
 * <table>
 *   <title>Types returned for each attribute</title>
 *   <thead>
 *     <tr>
 *       <th>Attributes</th>
 *       <th>Returned type</th>
 *     </tr>
 *   </thead>
 *   <tbody>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_PLACEMENT</code></td>
 *       <td>A #PopplerStructurePlacement value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_WRITING_MODE</code></td>
 *       <td>A #PopplerStructureWritingMode value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_TEXT_ALIGN</code></td>
 *       <td>A #PopplerStructureTextAlign, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_BLOCK_ALIGN</code></td>
 *       <td>A #PopplerStructureBlockAlign, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_INLINE_ALIGN</code></td>
 *       <td>A #PopplerStructureInlineAlign, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION</code></td>
 *       <td>A #PopplerStructureTextDecoration value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_RUBY_ALIGN</code></td>
 *       <td>A #PopplerStructureRubyAlign value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_RUBY_POSITION</code></td>
 *       <td>A #PopplerStructureRubyPosition value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_GLYPH_ORIENTATION</code></td>
 *       <td>A #PopplerStructureGlyphOrientation value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_LIST_NUMBERING</code></td>
 *       <td>A #PopplerStructureListNumbering value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_ROLE</code></td>
 *       <td>A #PopplerStructureRole value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_CHECKED</code></td>
 *       <td>A #PopplerStructureChecked value, as a <code>guint32</code>.</td>
 *     </tr><tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_SCOPE</code></td>
 *       <td>A #PopplerStructureScope value, as a <code>guint32</code>.</td>
 *     </tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_DESCRIPTION</code></td>
 *       <td rowspan="2">A string, as a <code>const gchar*</code>.</td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_SUMMARY</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_SPACE_BEFORE</code></td>
 *       <td rowspan="10">Number, as a <code>double</code>.</td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_SPACE_AFTER</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_START_INDENT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_END_INDENT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_TEXT_INDENT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_BASELINE_SHIFT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_THICKNESS</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_COUNT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_ROW_SPAN</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_SPAN</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_GAP</code></td>
 *       <td rowspan="2">
 *           An array of <code>double</code> numbers. The type of the
 *           returned #GVariant is <code>ad</code>.
 *       </td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_WIDTHS</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_WIDTH</code></td>
 *       <td rowspan="3">
 *           A maybe-double number. That is, a #GVariant with type
 *           <code>md</code>. If the number is undefined, the value
 *           is meant to be calculated automatically.
 *       </td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_HEIGHT</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_LINE_HEIGHT</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_COLOR</code></td>
 *       <td rowspan="3">
 *           A 3-tuple of doubles, with values in the <code>[0, 1]</code> range,
 *           in red-green-blue (RGB) order. The type of the returned #GVariant is
 *           <code>(ddd)</code>.
 *       </td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_BACKGROUND_COLOR</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_COLOR</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_BORDER_COLOR</code></td>
 *       <td>A 4-tuple of 3-tuples. Each one of the tuples is a RGB color,
 *           being each color component a double in the <code>[0, 1]</code>
 *           range. The four returned colors are in top-right-bottom-left
 *           order. The type of the returned #GVariant is
 *           <code>((ddd)(ddd)(ddd)(ddd))</code>.
 *       </td>
 *     </tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_BORDER_STYLE</code></td>
 *       <td rowspan="2">
 *           A 4-tuple of #PopplerStructureBorderStyle values, each one as a
 *           %guint32, in top-right-bottom-left order. The type of the
 *           returned #GVariant is <code>(uuuu)</code>.
 *       </td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_TABLE_BORDER_STYLE</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_BORDER_THICKNESS</code></td>
 *       <td rowspan="4">
 *           A 4-tuple of #double numbers, in top-right-bottom-left order.
 *           The type of the returned #GVariant is <code>(dddd)</code>.
 *       </td>
 *     </tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_TABLE_PADDING</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_PADDING</code></td></tr>
 *     <tr><td><code>POPPLER_STRUCTURE_ATTRIBUTE_BBOX</code></td></tr>
 *     <tr>
 *       <td><code>POPPLER_STRUCTURE_ATTRIBUTE_HEADERS</code></td>
 *       <td>An array of strings, each string being a <code>const gchar*</code>.
 *           The type of the returned #GVariant is <code>as</code>.</td>
 *     </tr>
 *   </tbody>
 * </table>
 *
 * Return value: (transfer full): A #GVariant, with value varying depending
 *    on the attribute requested, as specified in the table. If the
 *    attribute is not defined, <code>NULL</code> is returned.
 */
GVariant*
poppler_structure_element_get_attribute (PopplerStructureElement  *poppler_structure_element,
                                         PopplerStructureAttribute attribute,
                                         gboolean                  inherit)
{
  Object *value = NULL;

  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_return_val_if_fail (attribute != POPPLER_STRUCTURE_ATTRIBUTE_UNKNOWN, NULL);
  g_return_val_if_fail (attribute != POPPLER_STRUCTURE_ATTRIBUTE_USER_PROPERTY, NULL);

  switch (attribute)
    {
      case POPPLER_STRUCTURE_ATTRIBUTE_PLACEMENT:
        return name_to_variant_enum<PopplerStructurePlacement> (attr_value_or_default (poppler_structure_element,
                                                                                       Attribute::Placement, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_WRITING_MODE:
        return name_to_variant_enum<PopplerStructureWritingMode> (attr_value_or_default (poppler_structure_element,
                                                                                         Attribute::WritingMode, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_BACKGROUND_COLOR:
        value = attr_value_or_default (poppler_structure_element, Attribute::BackgroundColor, inherit);
        return (value && value->isArray ()) ? _g_variant_new_from_rgb_array (value->getArray ()) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_BORDER_COLOR:
        value = attr_value_or_default (poppler_structure_element, Attribute::BorderColor, inherit);
        return (value && value->isArray ()) ? _g_variant_new_from_rgb_array_or_x4 (value->getArray ()) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_BORDER_STYLE:
        value = attr_value_or_default (poppler_structure_element, Attribute::BorderStyle, inherit);
        return value ?_g_variant_new_from_border_style (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_BORDER_THICKNESS:
        value = attr_value_or_default (poppler_structure_element, Attribute::BorderThickness, inherit);
        return value ? _g_variant_new_from_number_or_x4 (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_COLOR:
        value = attr_value_or_default (poppler_structure_element, Attribute::Color, inherit);
        return (value && value->isArray ()) ? _g_variant_new_from_rgb_array (value->getArray ()) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_PADDING:
        value = attr_value_or_default (poppler_structure_element, Attribute::Padding, inherit);
        return value ? _g_variant_new_from_number_or_x4 (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_SPACE_BEFORE:
        value = attr_value_or_default (poppler_structure_element, Attribute::SpaceBefore, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_SPACE_AFTER:
        value = attr_value_or_default (poppler_structure_element, Attribute::SpaceAfter, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_START_INDENT:
        value = attr_value_or_default (poppler_structure_element, Attribute::StartIndent, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_END_INDENT:
        value = attr_value_or_default (poppler_structure_element, Attribute::EndIndent, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TEXT_INDENT:
        value = attr_value_or_default (poppler_structure_element, Attribute::TextIndent, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TEXT_ALIGN:
        return name_to_variant_enum<PopplerStructureTextAlign> (attr_value_or_default (poppler_structure_element,
                                                                                       Attribute::TextAlign, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_BBOX:
        value = attr_value_or_default (poppler_structure_element, Attribute::BBox, inherit);
        return value ? _g_variant_new_from_number_x4 (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_WIDTH:
        value = attr_value_or_default (poppler_structure_element, Attribute::Width, inherit);
        return value ? _g_variant_new_from_number_or_auto (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_HEIGHT:
        value = attr_value_or_default (poppler_structure_element, Attribute::Height, inherit);
        return value ? _g_variant_new_from_number_or_auto (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_BLOCK_ALIGN:
        return name_to_variant_enum<PopplerStructureBlockAlign> (attr_value_or_default (poppler_structure_element,
                                                                                        Attribute::BlockAlign, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_INLINE_ALIGN:
        return name_to_variant_enum<PopplerStructureInlineAlign> (attr_value_or_default (poppler_structure_element,
                                                                                         Attribute::InlineAlign, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_TABLE_BORDER_STYLE:
        value = attr_value_or_default (poppler_structure_element, Attribute::TBorderStyle, inherit);
        return value ?_g_variant_new_from_border_style (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TABLE_PADDING:
        value = attr_value_or_default (poppler_structure_element, Attribute::TPadding, inherit);
        return value ? _g_variant_new_from_number_or_x4 (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_BASELINE_SHIFT:
        value = attr_value_or_default (poppler_structure_element, Attribute::BaselineShift, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_LINE_HEIGHT:
        value = attr_value_or_default (poppler_structure_element, Attribute::LineHeight, inherit);
        return value ? _g_variant_new_from_number_or_auto_or_normal (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_COLOR:
        value = attr_value_or_default (poppler_structure_element, Attribute::TextDecorationColor, inherit);
        return (value && value->isArray ()) ? _g_variant_new_from_rgb_array (value->getArray ()) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION_THICKNESS:
        value = attr_value_or_default (poppler_structure_element, Attribute::LineHeight, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_TEXT_DECORATION:
        return name_to_variant_enum<PopplerStructureTextDecoration> (attr_value_or_default (poppler_structure_element,
                                                                                            Attribute::TextDecorationType, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_RUBY_ALIGN:
        return name_to_variant_enum<PopplerStructureRubyAlign> (attr_value_or_default (poppler_structure_element,
                                                                                       Attribute::RubyAlign, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_RUBY_POSITION:
        return name_to_variant_enum<PopplerStructureRubyPosition> (attr_value_or_default (poppler_structure_element,
                                                                                          Attribute::RubyPosition, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_GLYPH_ORIENTATION:
        return name_to_variant_enum<PopplerStructureGlyphOrientation> (attr_value_or_default (poppler_structure_element,
                                                                                              Attribute::GlyphOrientationVertical, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_COUNT:
        value = attr_value_or_default (poppler_structure_element, Attribute::ColumnCount, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_GAP:
        value = attr_value_or_default (poppler_structure_element, Attribute::ColumnGap, inherit);
        return value ? _g_variant_new_number_array (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_WIDTHS:
        value = attr_value_or_default (poppler_structure_element, Attribute::ColumnGap, inherit);
        return value ? _g_variant_new_number_array (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_LIST_NUMBERING:
        return name_to_variant_enum<PopplerStructureListNumbering> (attr_value_or_default (poppler_structure_element,
                                                                                           Attribute::ListNumbering, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_ROLE:
        return name_to_variant_enum<PopplerStructureRole> (attr_value_or_default (poppler_structure_element,
                                                                                  Attribute::Role, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_CHECKED:
        return name_to_variant_enum<PopplerStructureChecked> (attr_value_or_default (poppler_structure_element,
                                                                                     Attribute::checked, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_DESCRIPTION:
        return string_to_variant (attr_value_or_default (poppler_structure_element,
                                                         Attribute::Desc, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_ROW_SPAN:
        value = attr_value_or_default (poppler_structure_element, Attribute::RowSpan, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_COLUMN_SPAN:
        value = attr_value_or_default (poppler_structure_element, Attribute::ColSpan, inherit);
        return value ? _g_variant_new_from_number (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_HEADERS:
        value = attr_value_or_default (poppler_structure_element, Attribute::Headers, inherit);
        return value ? _g_variant_new_string_array (value) : NULL;

      case POPPLER_STRUCTURE_ATTRIBUTE_SCOPE:
        return name_to_variant_enum<PopplerStructureScope> (attr_value_or_default (poppler_structure_element,
                                                                                   Attribute::Scope, inherit));
      case POPPLER_STRUCTURE_ATTRIBUTE_SUMMARY:
        return string_to_variant (attr_value_or_default (poppler_structure_element, Attribute::Summary, inherit));

      case POPPLER_STRUCTURE_ATTRIBUTE_USER_PROPERTY:
      case POPPLER_STRUCTURE_ATTRIBUTE_UNKNOWN:
      default:
        g_assert_not_reached ();
        return NULL;
    }
}

/**
 * poppler_structure_element_is_reference:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: Whether the element is a reference to another object.
 */
gboolean
poppler_structure_element_is_reference (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), FALSE);
  g_assert (poppler_structure_element->elem);

  return poppler_structure_element->elem->isObjectRef ();
}

/**
 * poppler_structure_element_get_reference_type:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: The type of object pointed to by the reference, a value of
 *    #PopplerStructureReference.
 */
PopplerStructureReference
poppler_structure_element_get_reference_type (PopplerStructureElement *poppler_structure_element)
{
  PopplerStructureReference reftype = POPPLER_STRUCTURE_REFERENCE_UNKNOWN;

  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), reftype);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->isObjectRef ())
    {
      Object obj;
      const Ref ref = poppler_structure_element->elem->getObjectRef ();
      XRef *xref = poppler_structure_element->structure->document->doc->getXRef ();

      if (xref->fetch(ref.num, ref.gen, &obj)->isDict("Annot"))
        {
          reftype = POPPLER_STRUCTURE_REFERENCE_ANNOT;
          Object subtype;
          if (obj.dictLookup("Subtype", &subtype)->isName("Link"))
            reftype = POPPLER_STRUCTURE_REFERENCE_LINK;
          subtype.free();
        }

      obj.free();
    }

  return reftype;
}

/**
 * poppler_structure_element_get_reference_link:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer full): The #PopplerAnnot pointed by the object
 *    reference, or %NULL of the element is not a reference pointing to
 *    a #PopplerLink.
 */
PopplerLinkMapping*
poppler_structure_element_get_reference_link (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element_get_reference_type (poppler_structure_element)
      != POPPLER_STRUCTURE_REFERENCE_LINK)
    return NULL;

  gint num = poppler_structure_element_get_page (poppler_structure_element);
  if (num < 0 || num >= poppler_document_get_n_pages (poppler_structure_element->structure->document))
    return NULL;

  AnnotLink *link = NULL;
  Page *page = poppler_structure_element->structure->document->doc->getPage (num + 1);
  Links links(page->getAnnots ());

  for (gint i = 0; i < links.getNumLinks(); i++)
    {
      AnnotLink *l = links.getLink (i);
      const StructElement *parent = poppler_structure_element->structure->root->findParentElement (l->getTreeKey ());
      if (parent == poppler_structure_element->elem)
        {
          link = l;
          break;
        }
    }

  if (!link)
    return NULL;

  PopplerRectangle rect;
  LinkAction *link_action = link->getAction ();
  PopplerLinkMapping *mapping = poppler_link_mapping_new ();
  mapping->action = _poppler_action_new (poppler_structure_element->structure->document, link_action, NULL);

  link->getRect (&rect.x1, &rect.y1, &rect.x2, &rect.y2);

  rect.x1 -= page->getCropBox()->x1;
  rect.x2 -= page->getCropBox()->x1;
  rect.y1 -= page->getCropBox()->y1;
  rect.y2 -= page->getCropBox()->y1;

  switch (page->getRotate ())
    {
      case 90:
        mapping->area.x1 = rect.y1;
        mapping->area.y1 = page->getCropWidth () - rect.x2;
        mapping->area.x2 = mapping->area.x1 + (rect.y2 - rect.y1);
        mapping->area.y2 = mapping->area.y1 + (rect.x2 - rect.x1);
        break;
      case 180:
        mapping->area.x1 = page->getCropWidth () - rect.x2;
        mapping->area.y1 = page->getCropHeight () - rect.y2;
        mapping->area.x2 = mapping->area.x1 + (rect.x2 - rect.x1);
        mapping->area.y2 = mapping->area.y1 + (rect.y2 - rect.y1);
        break;
      case 270:
        mapping->area.x1 = page->getCropHeight () - rect.y2;
        mapping->area.y1 = rect.x1;
        mapping->area.x2 = mapping->area.x1 + (rect.y2 - rect.y1);
        mapping->area.y2 = mapping->area.y1 + (rect.x2 - rect.x1);
        break;
      default:
        mapping->area.x1 = rect.x1;
        mapping->area.y2 = rect.y1;
        mapping->area.x2 = rect.x2;
        mapping->area.y2 = rect.y2;
    }

  return mapping;
}

/**
 * poppler_structure_element_find_link:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer full): A #PopplerLinkMapping, or %NULL if the
 *    link cannot be found.
 */
PopplerLinkMapping*
poppler_structure_element_find_link (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->getType () != StructElement::Link)
    return NULL;

  for (unsigned i = 0; i < poppler_structure_element_get_n_children (poppler_structure_element); i++)
    {
      PopplerStructureElement *child = poppler_structure_element_get_child (poppler_structure_element, i);
      if (poppler_structure_element_get_reference_type (child) == POPPLER_STRUCTURE_REFERENCE_LINK)
        {
          PopplerLinkMapping *mapping = poppler_structure_element_get_reference_link (poppler_structure_element);
          if (mapping)
            return mapping;
        }
    }

  return NULL;
}

/**
 * poppler_structure_element_get_form_field:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer full): A #PopplerFormField, or %NULL if
 *    the element is not a %POPPLER_STRUCTURE_ELEMENT_FORM.
 */
PopplerFormField*
poppler_structure_element_get_form_field (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  if (poppler_structure_element->elem->getType () != StructElement::Form)
    return NULL;

  // TODO Handle elements which have a Role attribute (used sometimes for
  //      non-editable widgets, to describe their appearance). Editable
  //      fields have only a single child, with the field identifier.
  if (poppler_structure_element->elem->getNumElements () != 1)
    return NULL;

  gint field_id = -1;
  const StructElement *child = poppler_structure_element->elem->getElement (0);
  if (child->isContent ())
    {
      if (child->isObjectRef ())
        {
          // TODO Handle this case -- I have yet to see a PDF using this.
        }
      else
        {
          // Element contains the form field ID as the MCID attribute.
          field_id = child->getMCID ();
        }
    }

  if (field_id < 0)
    return NULL;

  return (field_id < 0) ? NULL : poppler_document_get_form_field (poppler_structure_element->structure->document,
                                                                  field_id);
}

/**
 * poppler_structure_element_get_form_field_mapping:
 * @poppler_structure_element: A #PopplerStructureElement
 *
 * Return value: (transfer full): A #PopplerFormFieldMapping, or %NULL if
 *    the element is not a %POPPLER_STRUCTURE_ELEMENT_FORM.
 */
PopplerFormFieldMapping*
poppler_structure_element_get_form_field_mapping (PopplerStructureElement *poppler_structure_element)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE_ELEMENT (poppler_structure_element), NULL);
  g_assert (poppler_structure_element->elem);

  gint page_num = poppler_structure_element_get_page (poppler_structure_element);
  g_assert (page_num < poppler_document_get_n_pages (poppler_structure_element->structure->document));
  if (page_num < 0)
    return NULL;

  Page *page = poppler_structure_element->structure->document->doc->getPage (page_num + 1);
  if (!page)
    return NULL;

  FormPageWidgets *forms = page->getFormWidgets ();
  if (!forms)
    return NULL;

  PopplerFormField *field = poppler_structure_element_get_form_field (poppler_structure_element);
  if (!field)
    return NULL;

  FormWidget *widget = NULL;
  for (int i = 0; i < forms->getNumWidgets (); i++)
    {
      FormWidget *w = forms->getWidget (i);
      if ((gint) w->getID () == poppler_form_field_get_id (field))
        {
          widget = w;
          break;
        }
    }

  if (!field)
    {
      g_object_unref (field);
      return NULL;
    }

  PopplerFormFieldMapping *mapping = poppler_form_field_mapping_new ();
  mapping->field = field;

  widget->getRect (&mapping->area.x1, &mapping->area.y1,
                   &mapping->area.x2, &mapping->area.y2);
  mapping->area.x1 -= page->getCropBox ()->x1;
  mapping->area.x2 -= page->getCropBox ()->x1;
  mapping->area.y1 -= page->getCropBox ()->y1;
  mapping->area.y2 -= page->getCropBox ()->y1;

  return mapping;
}
