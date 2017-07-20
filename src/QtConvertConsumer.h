#ifndef QTCONVERTCONSUMER_H
#define QTCONVERTCONSUMER_H

#include <clang/AST/ASTConsumer.h>

#include <QtConvertVisitor.h>

namespace clang
{
    class ASTContext;
}

class QtConvertConsumer : public clang::ASTConsumer {
public:
    explicit QtConvertConsumer(clang::ASTContext *Context);
    virtual void HandleTranslationUnit(clang::ASTContext &Context);
private:
    QtConvertVisitor Visitor;
};

#endif
