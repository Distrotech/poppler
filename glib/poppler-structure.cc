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
#endif /* !__GI_SCANNER__ */

#include "poppler.h"
#include "poppler-private.h"
#include "poppler-structure.h"


static inline StructElement::Type
_poppler_structure_element_kind_to_structelement_type (PopplerStructureElementKind kind)
{
  switch (kind)
    {
      case POPPLER_STRUCTURE_ELEMENT_UNKNOWN:
        return StructElement::Unknown;
      case POPPLER_STRUCTURE_ELEMENT_CONTENT:
        return StructElement::MCID;
      case POPPLER_STRUCTURE_ELEMENT_OBJECT_REFERENCE:
        return StructElement::OBJR;
      case POPPLER_STRUCTURE_ELEMENT_DOCUMENT:
        return StructElement::Document;
      case POPPLER_STRUCTURE_ELEMENT_PART:
        return StructElement::Part;
      case POPPLER_STRUCTURE_ELEMENT_SECTION:
        return StructElement::Sect;
      case POPPLER_STRUCTURE_ELEMENT_DIV:
        return StructElement::Div;
      case POPPLER_STRUCTURE_ELEMENT_SPAN:
        return StructElement::Span;
      case POPPLER_STRUCTURE_ELEMENT_QUOTE:
        return StructElement::Quote;
      case POPPLER_STRUCTURE_ELEMENT_NOTE:
        return StructElement::Note;
      case POPPLER_STRUCTURE_ELEMENT_REFERENCE:
        return StructElement::Reference;
      case POPPLER_STRUCTURE_ELEMENT_BIBENTRY:
        return StructElement::BibEntry;
      case POPPLER_STRUCTURE_ELEMENT_CODE:
        return StructElement::Code;
      case POPPLER_STRUCTURE_ELEMENT_LINK:
        return StructElement::Link;
      case POPPLER_STRUCTURE_ELEMENT_ANNOT:
        return StructElement::Annot;
      case POPPLER_STRUCTURE_ELEMENT_RUBY:
        return StructElement::Ruby;
      case POPPLER_STRUCTURE_ELEMENT_WARICHU:
        return StructElement::Warichu;
      case POPPLER_STRUCTURE_ELEMENT_BLOCKQUOTE:
        return StructElement::BlockQuote;
      case POPPLER_STRUCTURE_ELEMENT_CAPTION:
        return StructElement::Caption;
      case POPPLER_STRUCTURE_ELEMENT_NONSTRUCT:
        return StructElement::NonStruct;
      case POPPLER_STRUCTURE_ELEMENT_TOC:
        return StructElement::TOC;
      case POPPLER_STRUCTURE_ELEMENT_TOC_ITEM:
        return StructElement::TOCI;
      case POPPLER_STRUCTURE_ELEMENT_INDEX:
        return StructElement::Index;
      case POPPLER_STRUCTURE_ELEMENT_PRIVATE:
        return StructElement::Private;
      case POPPLER_STRUCTURE_ELEMENT_PARAGRAPH:
        return StructElement::P;
      case POPPLER_STRUCTURE_ELEMENT_HEADING:
        return StructElement::H;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_1:
        return StructElement::H1;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_2:
        return StructElement::H2;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_3:
        return StructElement::H3;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_4:
        return StructElement::H4;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_5:
        return StructElement::H5;
      case POPPLER_STRUCTURE_ELEMENT_HEADING_6:
        return StructElement::H6;
      case POPPLER_STRUCTURE_ELEMENT_LIST:
        return StructElement::L;
      case POPPLER_STRUCTURE_ELEMENT_LIST_ITEM:
        return StructElement::LI;
      case POPPLER_STRUCTURE_ELEMENT_LIST_LABEL:
        return StructElement::Lbl;
      case POPPLER_STRUCTURE_ELEMENT_TABLE:
        return StructElement::Table;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_ROW:
        return StructElement::TR;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_HEADING:
        return StructElement::TH;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_DATA:
        return StructElement::TD;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_HEADER:
        return StructElement::THead;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_FOOTER:
        return StructElement::TFoot;
      case POPPLER_STRUCTURE_ELEMENT_TABLE_BODY:
        return StructElement::TBody;
      case POPPLER_STRUCTURE_ELEMENT_FIGURE:
        return StructElement::Figure;
      case POPPLER_STRUCTURE_ELEMENT_FORMULA:
        return StructElement::Formula;
      case POPPLER_STRUCTURE_ELEMENT_FORM:
        return StructElement::Form;
      default:
        g_assert_not_reached ();
    }
}



/**
 * SECTION:poppler-structure
 * @short_description: Document structure
 * @title: PopplerStructure
 *
 * #PopplerStructure is used to represent the structure of a #PopplerDocument.
 * If a structure is defined, poppler_document_get_structure() will return a
 * valid pointer to its #PopplerStructure, which represents the document
 * tree. Elements can be obtained using poppler_structure_get_n_children()
 * and poppler_structure_get_child(). Seraching for elements of a given
 * #PopplerStructureElementType and/or page can be done using
 * poppler_structure_find_elements().
 *
 * The document structure tree is formed by #PopplerStructureElement objects,
 * describing each one of them a relevant element of the document. The
 * logical order of the documents is that of doing a depth-first traversal
 * of the tree. Elements may recursively contain other child elements, which
 * can be obtained using poppler_structure_element_get_n_children() and
 * poppler_structure_element_get_child().
 *
 * Elements may have attached attributes describing additional information
 * about them. The standard attributes (as defined in the PDF specification,
 * see #PopplerStructureAttribute for a complete list) can be obtained using
 * poppler_structure_element_get_attribute(). Elements may also contain
 * non-standard attributes with arbitrary names called “user properties”,
 * see poppler_structure_element_get_user_property() for details.
 */

typedef struct _PopplerStructureClass PopplerStructureClass;
struct _PopplerStructureClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (PopplerStructure, poppler_structure, G_TYPE_OBJECT);


static void
poppler_structure_init (PopplerStructure *poppler_structure)
{
}


static void
poppler_structure_finalize (GObject *object)
{
  PopplerStructure *poppler_structure = POPPLER_STRUCTURE (object);

  /* poppler_structure->root is owned by the catalog */
  g_object_unref (poppler_structure->document);

  if (poppler_structure->children)
    {
      for (unsigned i = 0; i < poppler_structure->root->getNumElements (); i++)
        g_object_unref (poppler_structure->children[i]);
      g_free (poppler_structure->children);
    }

  G_OBJECT_CLASS (poppler_structure_parent_class)->finalize (object);
}


static void
poppler_structure_class_init (PopplerStructureClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = poppler_structure_finalize;
}


PopplerStructure*
_poppler_structure_new (PopplerDocument *poppler_document,
                        StructTreeRoot  *root)
{
  PopplerStructure *poppler_structure;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (poppler_document), NULL);
  g_assert (root);

  poppler_structure = (PopplerStructure*) g_object_new (POPPLER_TYPE_STRUCTURE, NULL, NULL);

  poppler_structure->document = (PopplerDocument*) g_object_ref (poppler_document);
  poppler_structure->root     = root;
  poppler_structure->children = NULL;

  if (root->getNumElements ())
    poppler_structure->children = (PopplerStructureElement**) g_new0 (PopplerStructureElement*,
                                                                      root->getNumElements ());
  return poppler_structure;
}


/**
 * poppler_structure_get_n_children:
 * @poppler_structure: A #PopplerStructure
 *
 * Return value: Number of structure elements in the root of the
 *    structure tree.
 */
guint
poppler_structure_get_n_children (PopplerStructure *poppler_structure)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE (poppler_structure), 0);
  g_assert (poppler_structure->root);

  return poppler_structure->root->getNumElements ();
}

/**
 * poppler_structure_get_child:
 * @poppler_structure: A #PopplerStructure
 * @index: Index of the root structure element to obtain.
 *
 * Return value: (transfer none): A #PopplerStructureElement.
 */
PopplerStructureElement*
poppler_structure_get_child (PopplerStructure *poppler_structure,
                             guint             index)
{
  g_return_val_if_fail (POPPLER_IS_STRUCTURE (poppler_structure), NULL);
  g_assert (poppler_structure->root);
  g_assert (poppler_structure->root->getNumElements () >= 0);
  g_return_val_if_fail (index < (guint) poppler_structure->root->getNumElements (), NULL);

  if (!poppler_structure->children[index])
    {
      poppler_structure->children[index] = _poppler_structure_element_new (poppler_structure,
                                                                           poppler_structure->root->getElement (index));
      g_object_ref_sink (poppler_structure->children[index]);
    }
  return poppler_structure->children[index];
}


/*
 * XXX PopplerStructure and PopplerStructureElement cache their children
 *     PopplerStructureElement wrapper objects. This find function could
 *     traverse the cache instead of creating a list of new objects, but
 *     it is unclear whether that would be faster -- it will mostly depend
 *     on the use-case.
 */
static void
_poppler_structure_find_elements_helper (PopplerStructure    *structure,
                                         StructElement       *element,
                                         StructElement::Type  type,
                                         Ref                 *pageRef,
                                         GList              **result)
{
  g_assert (element);
  g_assert (result);

  /* Traverse the tree depth-first, to get elements in logical order */
  for (unsigned i = 0; i < element->getNumElements (); i++)
    {
      _poppler_structure_find_elements_helper (structure,
                                               element->getElement (i),
                                               type,
                                               pageRef,
                                               result);
    }

  /*
   * If filtering by type, when the type does not match,
   * return without adding the element to the result list.
   */
  if (type != StructElement::Unknown && (type != element->getType ()))
    return;

  /*
   * If filtering by page, when the page does not match,
   * return without adding the element to the result list.
   */
  if (pageRef)
    {
      Ref ref = element->getPageRef ();
      if (!(pageRef->num == ref.num && pageRef->gen == ref.gen))
        return;
    }

  /*
   * All filtering checks passed, add element to result list.
   */
  *result = g_list_append (*result, _poppler_structure_element_new (structure, element));
}

/**
 * poppler_structure_find_elements:
 * @poppler_structure: A #PopplerStructure.
 * @kind: A #PopplerStructureElementKind value. Use
 *   #POPPLER_STRUCTURE_ELEMENT_UNKNOWN to return all the structure
 *   elements, or any other value to obtain only the elements of the
 *   given type.
 * @page: Limit the elements returned to those whose content is to
 *   be displayed in a certain page. Use <code>-1</code> to obtain
 *   elements from all the pages.
 *
 * The elements are returned in *logical order* as defined in the
 * PDF specification, being that the ordering resulting of a depth-first
 * traversal of the structure tree.
 *
 * Return value: (element-type PopplerStructureElement) (transfer full):
 *   A #GList of #PopplerStructureElement objects.
 */
GList*
poppler_structure_find_elements (PopplerStructure           *poppler_structure,
                                 PopplerStructureElementKind kind,
                                 gint                        page)
{
  GList *result = NULL;
  Ref *pageRef = NULL;

  g_return_val_if_fail (POPPLER_IS_STRUCTURE (poppler_structure), NULL);

  if (page >= 0)
    {
      pageRef = poppler_structure->document->doc->getCatalog ()->getPageRef(page);
    }

  for (unsigned i = 0; i < poppler_structure->root->getNumElements (); i++)
    {
      _poppler_structure_find_elements_helper (poppler_structure,
                                               poppler_structure->root->getElement (i),
                                               _poppler_structure_element_kind_to_structelement_type (kind),
                                               pageRef,
                                               &result);
    }

  return result;
}
