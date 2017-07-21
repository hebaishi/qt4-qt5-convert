#ifndef METHODMATCHER_H
#define METHODMATCHER_H

#include <FunctionMatcher.h>

#include <functional>
#include <vector>

#include <clang/Basic/Specifiers.h>

namespace clang
{
    class QualType;
    class CXXMethodDecl;
}

typedef std::function<bool(const clang::QualType&)> QualTypeMatcher;

class MethodMatcher : public FunctionMatcher
{
public:
    MethodMatcher();
    MethodMatcher& matchClassName(const std::string& className);

    MethodMatcher& matchPublicMethod();
    MethodMatcher& matchProtectedMethod();
    MethodMatcher& matchPrivateMethod();

    bool isMatch(const clang::CXXMethodDecl* decl) const;

private:
    std::string m_className;
    clang::AccessSpecifier m_specifier;
};

#endif
