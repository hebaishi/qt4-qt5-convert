#include "QtConvertConsumer.h"

QtConvertConsumer::QtConvertConsumer(clang::ASTContext *Context)
    : Visitor(Context) {}

void QtConvertConsumer::HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    Visitor.dumpToOutputStream();
}
