#include <clang/AST/DeclBase.h>
#include <clang/AST/DeclCXX.h>

class NamespaceResolver
{
public:
    NamespaceResolver(const clang::LangOptions& langOptions);
    void visitDeclContext(clang::DeclContext* context);
    std::string ResolveType(const clang::QualType& t);

private:
    std::string getContextName(const clang::DeclContext* context);
    std::string getCleanName(const clang::QualType& type);
    clang::DeclContext* m_lastContext = nullptr;
    clang::LangOptions m_langOptions;
};
