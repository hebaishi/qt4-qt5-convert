#include <QtConvertAction.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    clang::tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                            OptionsParser.getSourcePathList());

    int result = Tool.run(clang::tooling::newFrontendActionFactory<QtConvertAction>().get());
    return result;
}
