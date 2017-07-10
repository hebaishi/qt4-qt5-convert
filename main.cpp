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

#include <NamespaceResolver.h>

std::string extractMethodCall(const std::string& literal)
{
    std::string result = literal;
    if (!result.empty())
    {
        result.erase(result.begin(), result.begin() + 1);
        auto parenPos = result.find('(');
        result = result.substr(0, parenPos);
    }
    return result;
}

using namespace clang;

class FindNamedClassVisitor
        : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
    explicit FindNamedClassVisitor(ASTContext *Context)
        : Context(Context),
          _rewriter(Context->getSourceManager(), Context->getLangOpts()),
          m_resolver(Context->getLangOpts())
    {}

    bool VisitNamespaceDecl(NamespaceDecl* context)
    {
        m_resolver.visitDeclContext(context);
        return true;
    }

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
                auto fullLocation = Context->getFullLoc(declaration->getLocStart());
                llvm::errs() << fullLocation.getFileEntry()->getName();
                llvm::errs() << ":" << fullLocation.getSpellingLineNumber();
                llvm::errs() << ":" << fullLocation.getSpellingColumnNumber() << " ";
                llvm::errs() << "Found " << className << "::" << methodName << "( ";
                llvm::errs() << declaration->getParamDecl(0)->getType().getAsString();
                for (int i=1 ; i < declaration->getNumParams() ; i++)
                {
                    llvm::errs() << ", " << declaration->getParamDecl(i)->getType().getAsString();
                }

                llvm::errs() << ")";
                myset.insert(declaration);
            }
            llvm::errs() << "\n";
        }
        return true;
    }

    bool VisitCallExpr(CallExpr *callExpression)
    {
        std::string methodCall, lastTypeString;
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
                                methodCall = extractMethodCall(literal->getBytes());
                            }
                        }
                    }

                    if (callExpression->getArg(i)->getLocEnd().isValid())
                    {

                        auto& sm = Context->getSourceManager();
                        if (callExpression->getArg(i)->getLocStart().isMacroID())
                        {
                            llvm::errs() << "Matching call argument " << i << " ";
                            auto fullStartLocation = Context->getSourceManager().getImmediateExpansionRange(callExpression->getArg(i)->getLocStart());
                            llvm::errs() << fullStartLocation.first.printToString(sm);
                            llvm::errs() << " to ";
                            llvm::errs() << fullStartLocation.second.printToString(sm);
                            llvm::errs() << "\n";

                            std::string replacementText = "&";
                            replacementText += lastTypeString;
                            replacementText += "::";
                            replacementText += methodCall;
                            _rewriter.ReplaceText(SourceRange(fullStartLocation.first, fullStartLocation.second), replacementText);
                            methodCall.clear();

                        }
                    }

                    QualType argumentType;
                    if (const ImplicitCastExpr* castExpr = dyn_cast<ImplicitCastExpr>(callExpression->getArg(i)))
                    {
                        if (const Expr* expr = dyn_cast<Expr>(*castExpr->child_begin()))
                        {
                            argumentType = expr->getType();
                        }
                    }
                    else
                    {
                        argumentType = callExpression->getArg(i)->getType();
                    }

                    lastTypeString = m_resolver.ResolveType(argumentType);

                    if (!lastTypeString.empty())
                        llvm::errs() << "Type string for parameter " << i << " is " << lastTypeString << "\n";
                }

            }
        }

        return true;
    }

    void dumpToOutputStream()
    {
        _rewriter.getEditBuffer(Context->getSourceManager().getMainFileID()).write(llvm::outs());
    }

private:
    ASTContext *Context;
    std::set <void*> myset;
    Rewriter _rewriter;
    std::vector<clang::tooling::Replacement> replacements;
    NamespaceResolver m_resolver;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
    explicit FindNamedClassConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        Visitor.dumpToOutputStream();
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

