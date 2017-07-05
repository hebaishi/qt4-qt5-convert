#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>

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
      if (callExpression->getDirectCallee())
      {
          if (myset.find(callExpression->getDirectCallee()->getFirstDecl()) != myset.end())
          {
              llvm::outs() << callExpression->getDirectCallee()->getNameAsString() << "\n";
              llvm::outs() << callExpression->getDirectCallee()->isCXXClassMember() <<"\n";
              llvm::outs() << callExpression->getDirectCallee()->getFirstDecl();

              clang::LangOptions LangOpts;
              LangOpts.CPlusPlus = true;
              clang::PrintingPolicy Policy(LangOpts);


              for(int i=0, j=callExpression->getNumArgs(); i<j; i++)
              {
                  if (const ImplicitCastExpr* castExpr = dyn_cast<ImplicitCastExpr>(callExpression->getArg(i)))
                  {
                      llvm::errs() << "Cast expression found:";
                      llvm::outs() << castExpr->getType().split().Ty->isPointerType();
                      llvm::errs() << "\n";
                  }
              }
          }
      }

      return true;
  }

private:
  ASTContext *Context;
  std::set <void*> myset;
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

