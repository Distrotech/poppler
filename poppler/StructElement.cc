//========================================================================
//
// StructElement.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2013 Igalia S.L.
//
//========================================================================

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "StructElement.h"
#include "StructTreeRoot.h"
#include "PDFDoc.h"
#include "Dict.h"

#include <assert.h>

class GfxState;


// Maps element types to their names and also serves as lookup table
// for additional element type attributes.

enum ElementType {
  elementTypeUndefined,
  elementTypeInline,
  elementTypeBlock,
};

static const struct TypeMapEntry {
  StructElement::Type       type;
  const char*               name;
  ElementType               elementType;
} typeMap[] = {
  { StructElement::Document,   "Document",   elementTypeInline    },
  { StructElement::Part,       "Part",       elementTypeInline    },
  { StructElement::Art,        "Art",        elementTypeInline    },
  { StructElement::Sect,       "Sect",       elementTypeInline    },
  { StructElement::Div,        "Div",        elementTypeInline    },
  { StructElement::BlockQuote, "BlockQuote", elementTypeInline    },
  { StructElement::Caption,    "Caption",    elementTypeInline    },
  { StructElement::NonStruct,  "NonStruct",  elementTypeInline    },
  { StructElement::Index,      "Index",      elementTypeInline    },
  { StructElement::Private,    "Private",    elementTypeInline    },
  { StructElement::Span,       "Span",       elementTypeInline    },
  { StructElement::Quote,      "Quote",      elementTypeInline    },
  { StructElement::Note,       "Note",       elementTypeInline    },
  { StructElement::Reference,  "Reference",  elementTypeInline    },
  { StructElement::BibEntry,   "BibEntry",   elementTypeInline    },
  { StructElement::Code,       "Code",       elementTypeInline    },
  { StructElement::Link,       "Link",       elementTypeInline    },
  { StructElement::Annot,      "Annot",      elementTypeInline    },
  { StructElement::Ruby,       "Ruby",       elementTypeInline    },
  { StructElement::RB,         "RB",         elementTypeUndefined },
  { StructElement::RT,         "RT",         elementTypeUndefined },
  { StructElement::RP,         "RP",         elementTypeUndefined },
  { StructElement::Warichu,    "Warichu",    elementTypeInline    },
  { StructElement::WT,         "WT",         elementTypeUndefined },
  { StructElement::WP,         "WP",         elementTypeUndefined },
  { StructElement::P,          "P",          elementTypeBlock     },
  { StructElement::H,          "H",          elementTypeBlock     },
  { StructElement::H1,         "H1",         elementTypeBlock     },
  { StructElement::H2,         "H2",         elementTypeBlock     },
  { StructElement::H3,         "H3",         elementTypeBlock     },
  { StructElement::H4,         "H4",         elementTypeBlock     },
  { StructElement::H5,         "H5",         elementTypeBlock     },
  { StructElement::H6,         "H6",         elementTypeBlock     },
  { StructElement::L,          "L",          elementTypeBlock     },
  { StructElement::LI,         "LI",         elementTypeBlock     },
  { StructElement::Lbl,        "Lbl",        elementTypeBlock     },
  { StructElement::Table,      "Table",      elementTypeBlock     },
  { StructElement::TR,         "TR",         elementTypeUndefined },
  { StructElement::TH,         "TH",         elementTypeUndefined },
  { StructElement::TD,         "TD",         elementTypeUndefined },
  { StructElement::THead,      "THead",      elementTypeUndefined },
  { StructElement::TFoot,      "TFoot",      elementTypeUndefined },
  { StructElement::TBody,      "TBody",      elementTypeUndefined },
  { StructElement::Figure,     "Figure",     elementTypeUndefined },
  { StructElement::Formula,    "Formula",    elementTypeUndefined },
  { StructElement::Form,       "Form",       elementTypeUndefined },
  { StructElement::TOC,        "TOC",        elementTypeUndefined },
  { StructElement::TOCI,       "TOCI",       elementTypeUndefined },
};


static inline const TypeMapEntry *getTypeMapEntry(StructElement::Type type)
{
  for (unsigned i = 0; i < sizeof(typeMap) / sizeof(typeMap[0]); i++) {
    if (type == typeMap[i].type)
      return &typeMap[i];
  }
  return NULL;
}

static inline const TypeMapEntry *getTypeMapEntry(const char *name)
{
  for (unsigned i = 0; i < sizeof(typeMap) / sizeof(typeMap[0]); i++) {
    if (strcmp(name, typeMap[i].name) == 0)
      return &typeMap[i];
  }
  return NULL;
}

static const char *typeToName(StructElement::Type type)
{
  if (type == StructElement::MCID)
    return "MarkedContent";
  if (type == StructElement::OBJR)
    return "ObjectReference";

  const TypeMapEntry *entry = getTypeMapEntry(type);
  return entry ? entry->name : "Unknown";
}

static StructElement::Type nameToType(const char *name)
{
  const TypeMapEntry *entry = getTypeMapEntry(name);
  return entry ? entry->type : StructElement::Unknown;
}


//------------------------------------------------------------------------
// StructElement
//------------------------------------------------------------------------

const Ref StructElement::InvalidRef = { -1, -1 };


StructElement::StructData::StructData():
  altText(0),
  actualText(0),
  id(0),
  title(0),
  expandedAbbr(0),
  language(0),
  revision(0)
{
}

StructElement::StructData::~StructData()
{
  delete altText;
  delete actualText;
  delete id;
  delete title;
  delete language;
  parentRef.free();
  for (ElemPtrArray::iterator i = elements.begin(); i != elements.end(); ++i) delete *i;
}


StructElement::StructElement(Dict *element,
                             StructTreeRoot *treeRootA,
                             StructElement *parentA,
                             std::set<int> &seen):
  type(Unknown),
  treeRoot(treeRootA),
  parent(parentA),
  s(new StructData())
{
  assert(treeRoot);
  assert(element);

  parse(element);
  parseChildren(element, seen);
}

StructElement::StructElement(int mcid, StructTreeRoot *treeRootA, StructElement *parentA):
  type(MCID),
  treeRoot(treeRootA),
  parent(parentA),
  c(new ContentData(mcid))
{
  assert(treeRoot);
  assert(parent);
  assert(c->mcid != InvalidMCID);
}

StructElement::StructElement(const Ref& ref, StructTreeRoot *treeRootA, StructElement *parentA):
  type(OBJR),
  treeRoot(treeRootA),
  parent(parentA),
  c(new ContentData(ref))
{
  assert(treeRoot);
  assert(parent);
  assert(c->ref.num >= 0);
  assert(c->ref.gen >= 0);
}

StructElement::~StructElement()
{
  if (isContent())
    delete c;
  else
    delete s;
  pageRef.free();
}

GBool StructElement::isBlock() const
{
  const TypeMapEntry *entry = getTypeMapEntry(type);
  return entry ? (entry->elementType == elementTypeBlock) : gFalse;
}

GBool StructElement::isInline() const
{
  const TypeMapEntry *entry = getTypeMapEntry(type);
  return entry ? (entry->elementType == elementTypeInline) : gFalse;
}

GBool StructElement::hasPageRef() const
{
  return pageRef.isRef() || (parent && parent->hasPageRef());
}

Ref StructElement::getPageRef() const
{
  if (pageRef.isRef())
    return pageRef.getRef();

  if (parent)
    return parent->getPageRef();

  static const Ref invalidRef = { -1, -1 };
  return invalidRef;
}

const char* StructElement::getTypeName() const
{
  return typeToName(type);
}

static StructElement::Type roleMapResolve(Dict *roleMap, const char *name, const char *curName, Object *resolved)
{
  // Circular reference
  if (curName && !strcmp(name, curName))
    return StructElement::Unknown;

  if (roleMap->lookup(curName ? curName : name, resolved)->isName()) {
    StructElement::Type type = nameToType(resolved->getName());
    return type == StructElement::Unknown
      ? roleMapResolve(roleMap, name, resolved->getName(), resolved)
      : type;
  }

  if (!resolved->isNull())
    error(errSyntaxWarning, -1, "RoleMap entry is wrong type ({0:s})", resolved->getTypeName());
  return StructElement::Unknown;
}

void StructElement::parse(Dict *element)
{
  Object obj;

  // Type is optional, but if present must be StructElem
  if (!element->lookup("Type", &obj)->isNull() && !obj.isName("StructElem")) {
    error(errSyntaxError, -1, "Type of StructElem object is wrong");
    obj.free();
    return;
  }
  obj.free();

  // Parent object reference (required).
  if (!element->lookupNF("P", &s->parentRef)->isRef()) {
    error(errSyntaxError, -1, "P object is wrong type ({0:s})", obj.getTypeName());
    return;
  }

  // Check whether the S-type is valid for the top level
  // element and create a node of the appropriate type.
  if (!element->lookup("S", &obj)->isName()) {
    error(errSyntaxError, -1, "S object is wrong type ({0:s})", obj.getTypeName());
    obj.free();
    return;
  }

  // Type name may not be standard, resolve through RoleMap first.
  if (treeRoot->getRoleMap()) {
    Object resolvedName;
    type = roleMapResolve(treeRoot->getRoleMap(), obj.getName(), NULL, &resolvedName);
  }

  // Resolving through RoleMap may leave type as Unknown, e.g. for types
  // which are not present in it, yet they are standard element types.
  if (type == Unknown)
    type = nameToType(obj.getName());

  // At this point either the type name must have been resolved.
  if (type == Unknown) {
    error(errSyntaxError, -1, "StructElem object is wrong type ({0:s})", obj.getName());
    obj.free();
    return;
  }
  obj.free();

  // Object ID (optional), to be looked at the IDTree in the tree root.
  if (element->lookup("ID", &obj)->isString()) {
    s->id = new GooString(obj.getString());
  }
  obj.free();

  // Page reference (optional) in which at least one of the child items
  // is to be rendered in. Note: each element stores only the /Pg value
  // contained by it, and StructElement::getPageRef() may look in parent
  // elements to find the page where an element belongs.
  element->lookupNF("Pg", &pageRef);

  // Revision number (optional).
  if (element->lookup("R", &obj)->isInt()) {
    s->revision = obj.getInt();
  }
  obj.free();

  // Element title (optional).
  if (element->lookup("T", &obj)->isString()) {
    s->title = new GooString(obj.getString());
  }
  obj.free();

  // Language (optional).
  if (element->lookup("Lang", &obj)->isString()) {
    s->language = obj.getString();
    obj.initNull(); // The StructElement takes ownership of the GooString
  }
  obj.free();

  // Alternative text (optional).
  if (element->lookup("Alt", &obj)->isString()) {
    s->altText = obj.getString();
    obj.initNull(); // The StructElement takes ownership of the GooString
  }
  obj.free();

  // Expanded form of an abbreviation (optional).
  if (element->lookup("E", &obj)->isString()) {
    s->expandedAbbr = obj.getString();
    obj.initNull(); // The StructElement takes ownership of the GooString
  }
  obj.free();

  // Actual text (optional).
  if (element->lookup("ActualText", &obj)->isString()) {
    s->actualText = obj.getString();
    obj.initNull(); // The StructElement takes ownership of the GooString
  }
  obj.free();

  // TODO: Attributes directly attached to the element (optional).
  // TODO: Attributes referenced indirectly through the ClassMap (optional).
}

StructElement *StructElement::parseChild(Object *ref,
                                         Object *childObj,
                                         std::set<int> &seen)
{
  assert(childObj);
  assert(ref);

  StructElement *child = NULL;

  if (childObj->isInt()) {
    child = new StructElement(childObj->getInt(), treeRoot, this);
  } else if (childObj->isDict("MCR")) {
    /*
     * TODO: The optional Stm/StwOwn attributes are not handled, so all the
     *      page will be always scanned when calling StructElement::getText().
     */
    Object mcidObj;
    Object pageRefObj;

    if (!childObj->dictLookup("MCID", &mcidObj)->isInt()) {
      error(errSyntaxError, -1, "MCID object is wrong type ({0:s})", mcidObj.getTypeName());
      mcidObj.free();
      return NULL;
    }

    child = new StructElement(mcidObj.getInt(), treeRoot, this);

    if (childObj->dictLookupNF("Pg", &pageRefObj)->isRef()) {
      child->pageRef = pageRefObj;
    } else {
      pageRefObj.free();
    }
  } else if (childObj->isDict("OBJR")) {
    Object refObj;

    if (childObj->dictLookupNF("Obj", &refObj)->isRef()) {
      Object pageRefObj;

      child = new StructElement(refObj.getRef(), treeRoot, this);

      if (childObj->dictLookupNF("Pg", &pageRefObj)->isRef()) {
        child->pageRef = pageRefObj;
      } else {
        pageRefObj.free();
      }
    } else {
      error(errSyntaxError, -1, "Obj object is wrong type ({0:s})", refObj.getTypeName());
    }
    refObj.free();
  } else if (childObj->isDict()) {
    assert(ref->isRef());
    if (seen.find(ref->getRefNum()) == seen.end()) {
      seen.insert(ref->getRefNum());
      child = new StructElement(childObj->getDict(), treeRoot, this, seen);
    } else {
      error(errSyntaxWarning, -1,
            "Loop detected in structure tree, skipping subtree at object {0:i}:{0:i}",
            ref->getRefNum(), ref->getRefGen());
    }
  } else {
    error(errSyntaxWarning, -1, "K has a child of wrong type ({0:s})", childObj->getTypeName());
  }

  if (child) {
    if (child->isOk()) {
      appendElement(child);
      if (ref->isRef())
        treeRoot->parentTreeAdd(ref->getRef(), child);
    } else {
      delete child;
      child = NULL;
    }
  }

  return child;
}

void StructElement::parseChildren(Dict *element, std::set<int> &seen)
{
  Object kids;

  if (element->lookup("K", &kids)->isArray()) {
    for (int i = 0; i < kids.arrayGetLength(); i++) {
      Object obj, ref;
      parseChild(kids.arrayGetNF(i, &ref), kids.arrayGet(i, &obj), seen);
      obj.free();
      ref.free();
    }
  } else if (kids.isDict() || kids.isInt()) {
    Object ref;
    parseChild(element->lookupNF("K", &ref), &kids, seen);
    ref.free();
  }

  kids.free();
}
