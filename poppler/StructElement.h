//========================================================================
//
// StructElement.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2013 Igalia S.L.
//
//========================================================================

#ifndef STRUCTELEMENT_H
#define STRUCTELEMENT_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "goo/gtypes.h"
#include "goo/GooString.h"
#include "Object.h"
#include <vector>

class GooString;
class Dict;
class StructTreeRoot;


// TODO: Dummy class to make it possible to build StructTreeRoot.cc
class StructElement {
public:
  enum Type {
    Unknown = 0,
    Document, Part, Art, Sect, Div,         // Structural elements
  };

  const char* getTypeName() const { return "dummy"; }
  Type getType() const { return type; }
  GBool isOk() const { return type != Unknown; }
  StructTreeRoot* getStructTreeRoot() { return treeRoot; }

  ~StructElement() {}

private:

  // Common data
  Type type;
  StructTreeRoot *treeRoot;
  StructElement *parent;

  StructElement(Dict *elementDict, StructTreeRoot *treeRootA, StructElement *parentA = 0) {}

  friend class StructTreeRoot;
};

#endif

