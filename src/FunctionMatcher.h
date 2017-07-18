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

class FunctionMatcher
{
public:
    FunctionMatcher();
    FunctionMatcher& addParameterMatcher(const QualTypeMatcher& func);
    FunctionMatcher& setReturnTypeMatcher(const QualTypeMatcher& methodName);
    FunctionMatcher& setMethodNameMatcher(const std::string& methodName);
    FunctionMatcher& setClassNameMatcher(const std::string& className);
    FunctionMatcher& setAccessSpecifierMatcher(clang::AccessSpecifier specifier);
    bool match(clang::CXXMethodDecl* decl) const;

private:
    bool matchArguments(clang::CXXMethodDecl* decl) const;
    std::vector<QualTypeMatcher> m_parameters;
    QualTypeMatcher m_returnType;
    std::string m_methodName;
    std::string m_className;
    clang::AccessSpecifier m_specifier;
};

#endif
