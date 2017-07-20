#include "QtConvertVisitor.h"

#include <CustomPrinter.h>
#include <TypeMatchers.h>

#include <clang/AST/DeclCXX.h>
#include <clang/AST/ASTContext.h>

QtConvertVisitor::QtConvertVisitor(clang::ASTContext *Context)
        : Context(Context),
          _rewriter(Context->getSourceManager(), Context->getLangOpts()),
          m_resolver(Context->getLangOpts())
{
    m_connectMatcher.matchClassName("QObject")
            .matchMethodName("connect")
            .matchReturnType(TypeMatchers::isQMetaObjectConnectionType)
            .matchAccessSpecifier(clang::AccessSpecifier::AS_public)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQtConnectionTypeEnum);
}

bool QtConvertVisitor::VisitNamespaceDecl(clang::NamespaceDecl* context)
{
    m_resolver.visitDeclContext(context);
    return true;
}

bool QtConvertVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* declaration)
{
    if (m_connectMatcher.isMatch(declaration))
    {
        CustomPrinter::printMethod(declaration);
        myset.insert(declaration);
    }
    return true;
}

bool QtConvertVisitor::VisitCallExpr(clang::CallExpr *callExpression)
{
    std::string methodCall, lastTypeString;
    if (callExpression->getDirectCallee())
    {
        if (myset.find(callExpression->getDirectCallee()->getFirstDecl()) != myset.end())
        {
            for (auto i = 0ul ; i < callExpression->getNumArgs() ; i++)
            {
                if (const clang::CallExpr *qflaglocationCall = clang::dyn_cast<clang::CallExpr>(callExpression->getArg(i)))
                {
                    if (const clang::ImplicitCastExpr *castExpr = clang::dyn_cast<clang::ImplicitCastExpr>(qflaglocationCall->getArg(0)))
                    {
                        if (const clang::StringLiteral* literal = clang::dyn_cast<clang::StringLiteral>(*castExpr->child_begin()))
                        {
                            methodCall = extractMethodCall(literal->getBytes());
                        }
                    }
                }

                if (callExpression->getArg(i)->getLocEnd().isValid())
                {

                    auto& sm = Context->getSourceManager();
                    if (callExpression->getArg(i)->getLocStart().isMacroID())
                    {
                        auto fullStartLocation = Context->getSourceManager().getImmediateExpansionRange(callExpression->getArg(i)->getLocStart());

                        std::string replacementText = "&";
                        replacementText += lastTypeString;
                        replacementText += "::";
                        replacementText += methodCall;
                        _rewriter.ReplaceText(clang::SourceRange(fullStartLocation.first, fullStartLocation.second), replacementText);
                        methodCall.clear();

                    }
                }

                clang::QualType argumentType;
                if (const clang::ImplicitCastExpr* castExpr = clang::dyn_cast<clang::ImplicitCastExpr>(callExpression->getArg(i)))
                {
                    if (const clang::Expr* expr = clang::dyn_cast<clang::Expr>(*castExpr->child_begin()))
                    {
                        argumentType = expr->getType();
                    }
                }
                else
                {
                    argumentType = callExpression->getArg(i)->getType();
                }

                lastTypeString = m_resolver.ResolveType(argumentType);
            }

        }
    }

    return true;
}

void QtConvertVisitor::dumpToOutputStream()
{
    _rewriter.getEditBuffer(Context->getSourceManager().getMainFileID()).write(llvm::outs());
}

std::string QtConvertVisitor::extractMethodCall(const std::string& literal)
{
    std::string result = literal;
    if (!result.empty())
    {
        result.erase(result.begin(), result.begin() + 1);
        auto parenPos = result.find('(');
        result = result.substr(0, parenPos);
    }
    return result;
}
