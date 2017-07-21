#ifndef FUNCTIONMATCHER_H
#define FUNCTIONMATCHER_H

#include <functional>
#include <vector>

namespace clang
{
    class QualType;
    class FunctionDecl;
}

typedef std::function<bool(const clang::QualType&)> QualTypeMatcher;

class FunctionMatcher
{
public:
    FunctionMatcher();
    FunctionMatcher& matchParameter(const QualTypeMatcher& func);
    FunctionMatcher& matchReturnType(const QualTypeMatcher& methodName);
    FunctionMatcher& matchFunctionName(const std::string& methodName);

    bool isMatch(const clang::FunctionDecl* decl) const;

private:
    bool matchArguments(const clang::FunctionDecl* decl) const;
    std::vector<QualTypeMatcher> m_parameters;
    QualTypeMatcher m_returnType;
    std::string m_FunctionName;
};

#endif
