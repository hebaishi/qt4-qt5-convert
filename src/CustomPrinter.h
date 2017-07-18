#ifndef CUSTOMPRINTER_H
#define CUSTOMPRINTER_H

namespace clang
{
    class CXXMethodDecl;
}

namespace CustomPrinter {
    void printMethod(const clang::CXXMethodDecl* declaration);
}

#endif
