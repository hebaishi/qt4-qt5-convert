#include "CustomPrinter.h"

#include <clang/AST/DeclCXX.h>

namespace CustomPrinter
{

void printMethod(const clang::CXXMethodDecl *declaration)
{
    auto methodName = declaration->getAsFunction()->getNameInfo().getAsString();
    auto className = declaration->getParent()->getNameAsString();
    auto returnType = declaration->getReturnType().getAsString();
    auto& Context = declaration->getASTContext();
    auto fullLocation = Context.getFullLoc(declaration->getLocStart());

    llvm::errs()
        << fullLocation.getFileEntry()->getName()
        << ":" << fullLocation.getSpellingLineNumber()
        << ":" << fullLocation.getSpellingColumnNumber() << ": "
        << "Found " << returnType << " "
        << className << "::" << methodName << " (";
    if (declaration->getNumParams() > 0)
    {
        llvm::errs() << declaration->getParamDecl(0)->getType().getAsString();
        for (auto i=1ul ; i < declaration->getNumParams() ; i++)
        {
            llvm::errs() << ", " << declaration->getParamDecl(i)->getType().getAsString();
        }
    }

    llvm::errs() << ")\n";
}

}
