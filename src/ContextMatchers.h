#ifndef CONTEXTMATCHERS_H
#define CONTEXTMATCHERS_H
#include <clang/AST/Decl.h>

bool isEnumInQtNamespace(const clang::QualType& type);

#endif
