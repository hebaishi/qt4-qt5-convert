#include "MethodMatcher.h"

#include <TypeMatchers.h>

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>

MethodMatcher::MethodMatcher() :
    m_returnType(TypeMatchers::isVoidType),
    m_specifier(clang::AccessSpecifier::AS_none)
{}

MethodMatcher &MethodMatcher::matchParameter(const QualTypeMatcher &func)
{
    m_parameters.push_back(func);
    return *this;
}

MethodMatcher &MethodMatcher::matchReturnType(const QualTypeMatcher &methodName)
{
    m_returnType = methodName;
    return *this;
}

MethodMatcher &MethodMatcher::matchMethodName(const std::string &methodName)
{
    m_methodName = methodName;
    return *this;
}

MethodMatcher &MethodMatcher::matchClassName(const std::string &className)
{
    m_className = className;
    return *this;
}

MethodMatcher &MethodMatcher::matchAccessSpecifier(clang::AccessSpecifier specifier)
{
    m_specifier = specifier;
    return *this;
}

bool MethodMatcher::isMatch(clang::CXXMethodDecl *decl) const
{
    assert(!m_methodName.empty());
    assert(!m_className.empty());
    assert(!(m_specifier == clang::AccessSpecifier::AS_none));

    return decl->getAccess() == m_specifier
            && decl->getAsFunction()->getNameInfo().getAsString() == m_methodName
            && decl->getParent()->getNameAsString() == m_className
            && m_returnType(decl->getReturnType())
            && matchArguments(decl);
}

bool MethodMatcher::matchArguments(clang::CXXMethodDecl *decl) const
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
