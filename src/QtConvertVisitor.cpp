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
    setupMatchers();
}

bool QtConvertVisitor::VisitNamespaceDecl(clang::NamespaceDecl* context)
{
    m_resolver.visitDeclContext(context);
    return true;
}

bool QtConvertVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* declaration)
{
    for (const auto& methodMatcher : m_connectMatchers)
    {
        if (methodMatcher.isMatch(declaration))
        {
            CustomPrinter::printMethod(declaration);
            myset.insert(declaration);
        }
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
            for (const auto& argument : callExpression->arguments())
            {
                auto argumentType = argument->getType();

                if (TypeMatchers::isQObjectPtrType(argumentType))
                {
                    clang::QualType argumentType = getRealArgumentType(argument);
                    lastTypeString = m_resolver.ResolveType(argumentType);
                }
                else if (TypeMatchers::isConstCharPtrType(argumentType))
                {
                    methodCall = getMethodCallIfPresent(argument);
                    if (argument->getLocStart().isMacroID() && !methodCall.empty())
                    {
                        addReplacement(argument, lastTypeString, methodCall);
                    }
                }
            }
        }
    }
    return true;
}

void QtConvertVisitor::dumpToOutputStream()
{
    _rewriter.getEditBuffer(Context->getSourceManager().getMainFileID()).write(llvm::outs());
}

clang::QualType QtConvertVisitor::getRealArgumentType(const clang::Expr *expression)
{
    if (const clang::ImplicitCastExpr* castExpr = clang::dyn_cast<clang::ImplicitCastExpr>(expression))
    {
        if (const clang::Expr* expr = clang::dyn_cast<clang::Expr>(*castExpr->child_begin()))
        {
            return expr->getType();
        }
    }
    return expression->getType();
}

std::string QtConvertVisitor::getMethodCallIfPresent(const clang::Expr *expression)
{
    if (const clang::CallExpr *qflaglocationCall = clang::dyn_cast<clang::CallExpr>(expression))
    {
        auto callee = qflaglocationCall->getDirectCallee();
        if (callee)
        {
            if (m_qFlagLocationMatcher.isMatch(callee->getFirstDecl()))
            {
                if (const clang::ImplicitCastExpr *castExpr = clang::dyn_cast<clang::ImplicitCastExpr>(qflaglocationCall->getArg(0)))
                {
                    if (const clang::StringLiteral* literal = clang::dyn_cast<clang::StringLiteral>(*castExpr->child_begin()))
                    {
                        return extractMethodCall(literal->getBytes());
                    }
                }
            }
        }
    }
    return std::string();
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

void QtConvertVisitor::addReplacement(const clang::Expr *expression, const std::string &typeString, const std::string &methodCall)
{
    auto fullStartLocation = Context->getSourceManager().getImmediateExpansionRange(
                expression->getLocStart());

    std::string replacementText = "&";
    replacementText += typeString;
    replacementText += "::";
    replacementText += methodCall;
    _rewriter.ReplaceText(clang::SourceRange(
                              fullStartLocation.first, fullStartLocation.second
                              ), replacementText);
}

void QtConvertVisitor::setupMatchers()
{
    m_connectMatchers.push_back(MethodMatcher());
    m_connectMatchers.back().matchClassName("QObject")
            .matchPublicMethod()
            .matchFunctionName("connect")
            .matchReturnType(TypeMatchers::isQMetaObjectConnectionType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQtConnectionTypeEnum);

    m_connectMatchers.push_back(MethodMatcher());
    m_connectMatchers.back().matchClassName("QObject")
            .matchPublicMethod()
            .matchFunctionName("connect")
            .matchReturnType(TypeMatchers::isQMetaObjectConnectionType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQtConnectionTypeEnum);

    m_connectMatchers.push_back(MethodMatcher());
    m_connectMatchers.back().matchClassName("QObject")
            .matchPublicMethod()
            .matchFunctionName("disconnect")
            .matchReturnType(TypeMatchers::isBoolType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType);

    m_connectMatchers.push_back(MethodMatcher());
    m_connectMatchers.back().matchClassName("QObject")
            .matchPublicMethod()
            .matchFunctionName("disconnect")
            .matchReturnType(TypeMatchers::isBoolType)
            .matchParameter(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isQObjectPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType);


    m_qFlagLocationMatcher.matchFunctionName("qFlagLocation")
            .matchReturnType(TypeMatchers::isConstCharPtrType)
            .matchParameter(TypeMatchers::isConstCharPtrType);
}
