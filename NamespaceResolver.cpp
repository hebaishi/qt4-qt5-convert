#include "NamespaceResolver.h"

NamespaceResolver::NamespaceResolver(const clang::LangOptions &langOptions) :
    m_langOptions(langOptions)
{}

void NamespaceResolver::visitDeclContext(clang::DeclContext* context)
{
    m_lastContext = context;
}

std::string NamespaceResolver::ResolveType(const clang::QualType& t)
{
    std::string result = getCleanName(t);
    if (!m_lastContext) return result;
    const clang::CXXRecordDecl* typeDecl = t.getTypePtr()->getPointeeCXXRecordDecl();
    if (typeDecl)
    {
        auto currentScope = typeDecl->getLookupParent();
        auto typeScope = m_lastContext->getLookupParent();


        std::vector<std::string> namespaceDecls;

        if (!typeScope || !currentScope) return result;

        while ( ( !( currentScope->Equals(typeScope) || currentScope->isTranslationUnit() ) ) )
        {
            namespaceDecls.push_back(getContextName(currentScope));
            currentScope = currentScope->getLookupParent();
        }

        if (!namespaceDecls.empty())
            namespaceDecls.erase( namespaceDecls.begin() + namespaceDecls.size() - 1);

        for (const auto& decl : namespaceDecls)
        {
            result = decl + "::" + result;
        }
    }
    return result;
}

std::string NamespaceResolver::getContextName(const clang::DeclContext* context)
{
    if (const clang::NamespaceDecl* decl = clang::dyn_cast<clang::NamespaceDecl>(context))
    {
        return decl->getNameAsString();
    }
    else if (const clang::CXXRecordDecl* decl = clang::dyn_cast<clang::CXXRecordDecl>(context))
    {
        return decl->getNameAsString();
    }
    return std::string();
}

std::string NamespaceResolver::getCleanName(const clang::QualType &value)
{
    clang::QualType typeCopy = value;
    if (typeCopy.getTypePtr()->isPointerType())
    {
        typeCopy = typeCopy.getTypePtr()->getPointeeType();
    }
    typeCopy.removeLocalConst();

    clang::PrintingPolicy policy(m_langOptions);
    policy.SuppressUnwrittenScope = 1;
    policy.TerseOutput = 1;
    policy.PolishForDeclaration = 1;
    policy.SuppressScope = 1;

    std::string result;
    result.clear();
    typeCopy.getAsStringInternal(result, policy);
    return result;
}
