#include "QtConvertAction.h"

#include <QtConvertConsumer.h>

#include <clang/Frontend/CompilerInstance.h>

std::unique_ptr<clang::ASTConsumer> QtConvertAction::CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
                new QtConvertConsumer(&Compiler.getASTContext()));
}
