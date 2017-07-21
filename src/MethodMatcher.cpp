#include "MethodMatcher.h"

#include <TypeMatchers.h>

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>

MethodMatcher::MethodMatcher() :
    m_specifier(clang::AccessSpecifier::AS_none)
{}

MethodMatcher &MethodMatcher::matchClassName(const std::string &className)
{
    m_className = className;
    return *this;
}

MethodMatcher &MethodMatcher::matchPublicMethod()
{
    m_specifier = clang::AccessSpecifier::AS_public;
    return *this;
}

MethodMatcher &MethodMatcher::matchProtectedMethod()
{
    m_specifier = clang::AccessSpecifier::AS_protected;
    return *this;
}

MethodMatcher &MethodMatcher::matchPrivateMethod()
{
    m_specifier = clang::AccessSpecifier::AS_private;
    return *this;
}

bool MethodMatcher::isMatch(const clang::CXXMethodDecl *decl) const
{
    assert(!m_className.empty());
    assert(!(m_specifier == clang::AccessSpecifier::AS_none));

    return FunctionMatcher::isMatch(decl)
            && decl->getAccess() == m_specifier
            && decl->getParent()->getNameAsString() == m_className;
}
