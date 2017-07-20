#ifndef QTCONVERTACTION
#define QTCONVERTACTION

#include <clang/Frontend/FrontendAction.h>

#include <memory>

namespace clang
{
    class ASTConsumer;
    class CompilerInstance;
}

class QtConvertAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile);
};

#endif
