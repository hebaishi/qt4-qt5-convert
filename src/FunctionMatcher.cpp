#include "FunctionMatcher.h"

#include <TypeMatchers.h>

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>

FunctionMatcher::FunctionMatcher() :
    m_returnType(TypeMatchers::isVoidType)
{}

FunctionMatcher &FunctionMatcher::matchParameter(const QualTypeMatcher &func)
{
    m_parameters.push_back(func);
    return *this;
}

FunctionMatcher &FunctionMatcher::matchReturnType(const QualTypeMatcher &methodName)
{
    m_returnType = methodName;
    return *this;
}

FunctionMatcher &FunctionMatcher::matchFunctionName(const std::string &methodName)
{
    m_FunctionName = methodName;
    return *this;
}

bool FunctionMatcher::isMatch(const clang::FunctionDecl *decl) const
{
    assert(!m_FunctionName.empty());

    return decl->getAsFunction()->getNameInfo().getAsString() == m_FunctionName
            && m_returnType(decl->getReturnType())
            && matchArguments(decl);
}

bool FunctionMatcher::matchArguments(const clang::FunctionDecl *decl) const
{
    if (decl->getAsFunction()->getNumParams() == m_parameters.size())
    {
        for (auto idx = 0ul ; idx < m_parameters.size() ; idx++)
            if (!m_parameters[idx](decl->getParamDecl(idx)->getType()))
                return false;

        return true;
    }
    else
    {
        return false;
    }
}
