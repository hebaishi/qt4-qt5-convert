#ifndef FUNCTIONMATCHER_H
#define FUNCTIONMATCHER_H

#include <functional>
#include <vector>

#include <clang/Basic/Specifiers.h>

namespace clang
{
    class QualType;
    class CXXMethodDecl;
}

typedef std::function<bool(const clang::QualType&)> QualTypeMatcher;

class MethodMatcher
{
public:
    MethodMatcher();
    MethodMatcher& matchParameter(const QualTypeMatcher& func);
    MethodMatcher& matchReturnType(const QualTypeMatcher& methodName);
    MethodMatcher& matchMethodName(const std::string& methodName);
    MethodMatcher& matchClassName(const std::string& className);
    MethodMatcher& matchAccessSpecifier(clang::AccessSpecifier specifier);
    bool isMatch(clang::CXXMethodDecl* decl) const;

private:
    bool matchArguments(clang::CXXMethodDecl* decl) const;
    std::vector<QualTypeMatcher> m_parameters;
    QualTypeMatcher m_returnType;
    std::string m_methodName;
    std::string m_className;
    clang::AccessSpecifier m_specifier;
};

#endif
