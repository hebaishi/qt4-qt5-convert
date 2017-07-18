#include "FunctionMatcher.h"

#include <TypeMatchers.h>

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>

FunctionMatcher::FunctionMatcher() :
    m_returnType(TypeMatchers::isVoidType),
    m_specifier(clang::AccessSpecifier::AS_none)
{}

FunctionMatcher &FunctionMatcher::addParameterMatcher(const QualTypeMatcher &func)
{
    m_parameters.push_back(func);
    return *this;
}

FunctionMatcher &FunctionMatcher::setReturnTypeMatcher(const QualTypeMatcher &methodName)
{
    m_returnType = methodName;
    return *this;
}

FunctionMatcher &FunctionMatcher::setMethodNameMatcher(const std::string &methodName)
{
    m_methodName = methodName;
    return *this;
}

FunctionMatcher &FunctionMatcher::setClassNameMatcher(const std::string &className)
{
    m_className = className;
    return *this;
}

FunctionMatcher &FunctionMatcher::setAccessSpecifierMatcher(clang::AccessSpecifier specifier)
{
    m_specifier = specifier;
    return *this;
}

bool FunctionMatcher::match(clang::CXXMethodDecl *decl) const
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

bool FunctionMatcher::matchArguments(clang::CXXMethodDecl *decl) const
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
