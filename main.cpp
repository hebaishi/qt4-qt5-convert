#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>

#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Core/Replacement.h>

#include <TypeMatchers.h>

#include <iostream>
#include <set>

using namespace clang;

class FindNamedClassVisitor
        : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
    explicit FindNamedClassVisitor(ASTContext *Context)
        : Context(Context) {}

    bool VisitCXXMethodDecl(CXXMethodDecl* declaration)
    {
        auto methodName = declaration->getAsFunction()->getNameInfo().getAsString();
        auto className = declaration->getParent()->getNameAsString();
        if (methodName == "connect"
                && className == "QObject"
                && declaration->getAccess() == AccessSpecifier::AS_public
                && declaration->getNumParams() >= 3)
        {
            bool isFirstCorrect = isQObjectPtrType(declaration->getParamDecl(0));
            bool isSecondCorrect = isConstCharPtrType(declaration->getParamDecl(1));

            if (isFirstCorrect && isSecondCorrect)
            {
                llvm::outs() << "Found " << className << "::" << methodName << "( ";
                llvm::outs() << declaration->getParamDecl(0)->getType().getAsString();
                for (int i=1 ; i < declaration->getNumParams() ; i++)
                {
                    llvm::outs() << ", " << declaration->getParamDecl(i)->getType().getAsString();
                }

                llvm::outs() << ")";
                auto fullLocation = Context->getFullLoc(declaration->getLocStart());
                llvm::outs() << " at " << fullLocation.getSpellingLineNumber();
                llvm::outs() << ":" << fullLocation.getSpellingColumnNumber();
                llvm::outs() << " in file" << fullLocation.getFileEntry()->getName() << "\n";
                myset.insert(declaration);
            }
        }
        return true;
    }


    bool VisitCXXMemberCallExpr(CXXMemberCallExpr *callExpression)
    {
        //      llvm::outs() << callExpression->getDirectCallee()->getAsFunction()->getDeclName() << "\n";
        //      if (callExpression->getDirectCallee()->getNameInfo().getName().getAsString() == "connect"
        //        && callExpression->getDecl()->getParent()->isClass()
        //        && callExpression->getDecl()->getParent()->getNameAsString() == "QObject")
        //    {
        //        FullSourceLoc FullLocation = Context->getFullLoc(callExpression->getLocStart());
        //        if (FullLocation.isValid())
        //          llvm::outs() << "Found declaration at "
        //                       << FullLocation.getSpellingLineNumber() << ":"
        //                       << FullLocation.getSpellingColumnNumber() << " in file "
        //                       << FullLocation.getFileEntry()->getName() << "\n";
        //    }
        return true;

    }

    bool VisitCallExpr(CallExpr *callExpression)
    {
        Rewriter fileWriter(Context->getSourceManager(), Context->getLangOpts());
        if (callExpression->getDirectCallee())
        {
            if (myset.find(callExpression->getDirectCallee()->getFirstDecl()) != myset.end())
            {
                for (int i = 0 ; i < callExpression->getNumArgs() ; i++)
                {
                    if (const CallExpr *qflaglocationCall = dyn_cast<CallExpr>(callExpression->getArg(i)))
                    {
                        if (const ImplicitCastExpr *castExpr = dyn_cast<ImplicitCastExpr>(qflaglocationCall->getArg(0)))
                        {
                            if (const StringLiteral* literal = dyn_cast<StringLiteral>(*castExpr->child_begin()))
                            {
                                llvm::outs() << "Found string!! " << literal->getBytes() << "\n";
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

private:
    ASTContext *Context;
    std::set <void*> myset;
    std::vector<clang::tooling::Replacement> replacements;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
    explicit FindNamedClassConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                    new FindNamedClassConsumer(&Compiler.getASTContext()));
    }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    const std::string fName = "doSomething";

    tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    tooling::ClangTool Tool(OptionsParser.getCompilations(),
                            OptionsParser.getSourcePathList());

    // run the Clang Tool, creating a new FrontendAction (explained below)
    int result = Tool.run(clang::tooling::newFrontendActionFactory<FindNamedClassAction>().get());
    return result;
}

