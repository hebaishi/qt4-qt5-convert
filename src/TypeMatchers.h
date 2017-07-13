#ifndef TYPEMATCHERS_H
#define TYPEMATCHERS_H

#include <clang/AST/Decl.h>

bool isConstCharPtrType(const clang::QualType& type);
bool isQObjectPtrType(const clang::QualType& type);
bool isQtConnectionTypeEnum(const clang::QualType& type);

#endif
