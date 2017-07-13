#include "ContextMatchers.h"
#include <clang/AST/DeclCXX.h>

bool isEnumInQtNamespace(const clang::QualType& type)
{
    auto tagDecl = type.getTypePtr()->getAsTagDecl();
    if (tagDecl)
    {
        if (const clang::EnumDecl* enumDecl = clang::dyn_cast<clang::EnumDecl>(tagDecl))
        {
            auto lookupParent = enumDecl->getLookupParent();
            if (const clang::NamedDecl* namespaceDecl = clang::dyn_cast<clang::NamespaceDecl>(lookupParent))
                return namespaceDecl->getNameAsString() == "Qt";
        }
    }
    return false;
}
