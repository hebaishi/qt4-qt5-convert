#ifndef QTCONVERTVISITOR_H
#define QTCONVERTVISITOR_H

#include <NamespaceResolver.h>
#include <MethodMatcher.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Core/Replacement.h>

#include <set>

namespace clang
{
    class ASTContext;
    class NamespaceDecl;
    class CXXMethodDecl;
    class CallExpr;
}

class QtConvertVisitor : public clang::RecursiveASTVisitor<QtConvertVisitor>
{
public:
    explicit QtConvertVisitor(clang::ASTContext *Context);
    bool VisitNamespaceDecl(clang::NamespaceDecl* context);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl* declaration);
    bool VisitCallExpr(clang::CallExpr *callExpression);
    void dumpToOutputStream();

private:
    clang::QualType getRealArgumentType(const clang::Expr* expression);
    std::string getMethodCallIfPresent(const clang::Expr* expression);
    std::string extractMethodCall(const std::string& literal);
    void addReplacement(const clang::Expr* expression,
                               const std::string& typeString, const std::string& methodCall);

    clang::ASTContext *Context;
    std::set <void*> myset;
    clang::Rewriter _rewriter;
    std::vector<clang::tooling::Replacement> replacements;
    NamespaceResolver m_resolver;
    MethodMatcher m_connectMatcher;
};

#endif
