//
// Created by Saikat Chakraborty on 9/24/20.
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Lex/Lexer.h"
#include <iostream>


using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
static cl::OptionCategory FindFunctionCategory("clang-hw2 options");

/*
 * This helper function is formal printing api for recursive nature of a function.
 */
void printFunction(const std::string& functionName, bool recursive){
    std::cout << functionName << "\tis a ";
    if(!recursive){
        std::cout << "non-";
    }
    std::cout << "recursive function\n";
}


/*
 * This helper function prints out previous declaration of a variable.
 */
void printVarReDeclarationInformation(std::string& varName,
                                      int currentDefLineNo, int initialDefLineNo){
    std::cout << "Redefining variable : \"" << varName << "\" at line "
              << currentDefLineNo << " which is initially defined at line "
              << initialDefLineNo << std::endl;
}

/*
 * This function prints Tree structure of a given AST node.
 */
void prettyPrintASTNode(Stmt* node, int numTab){
    for(int i = 0; i < numTab; i++){
        std::cerr << "\t";
    }
    std::cerr << node->getStmtClassName() << std::endl;
    for(auto child : node->children()){
        if(child != nullptr){
            prettyPrintASTNode(child, numTab+1);
        }
    }
}

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
public:
    explicit FunctionVisitor(ASTContext* _context, CompilerInstance& _compiler)
            : context(_context), compiler(_compiler) {}

    bool isRecursiveFunction(FunctionDecl* function, Stmt* stmt){
        // TODO Implement this function.
        return false;
    }

    bool VisitFunctionDecl(FunctionDecl* function) {
        std::string functionName = function->getNameInfo().getName().getAsString();
        if(function->hasBody()){
            Stmt* body = function->getBody();
            printFunction(functionName, isRecursiveFunction(function, body));
        }
        else{
            printFunction(functionName, false);
        }
        return true;
    }

    bool VisitVarDecl(VarDecl *var){
        // TODO : Implement the inner scope redefinition warning policy here.
        return true;
    }

    /*
     * This function is called when the visitor encounters a For statement in the AST.
     */
    bool VisitForStmt(ForStmt* forStmt) {
        // We will give them this code for a reference code.
        assert(isa<ForStmt>(forStmt));
        FullSourceLoc fullSourceLoc = context->getFullLoc(forStmt->getBeginLoc());
        if(!fullSourceLoc.isValid()) return false;
        int ln = fullSourceLoc.getSpellingLineNumber();
        std::cerr << "======================================\n";
        std::cerr << "Found For Statement at line : " << ln << "\n";
        prettyPrintASTNode(forStmt, 0);
        std::cerr << "======================================\n";
        return true;
    }

private:
    ASTContext* context;
    CompilerInstance& compiler;
};

class FunctionVisitorConsumer : public clang::ASTConsumer {
public:
    explicit FunctionVisitorConsumer(ASTContext* context, CompilerInstance& compiler)
            : visitor(context, compiler) {}

    virtual void HandleTranslationUnit(clang::ASTContext& context) {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    FunctionVisitor visitor;
};

class FunctionVisitAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance& compiler, llvm::StringRef inFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new FunctionVisitorConsumer(&compiler.getASTContext(), compiler));
    }
};

int main(int argc, const char **argv) {
    CommonOptionsParser optionsParser(argc, argv, FindFunctionCategory);
    ClangTool tool(optionsParser.getCompilations(),
                   optionsParser.getSourcePathList());
    return tool.run(newFrontendActionFactory<FunctionVisitAction>().get());
}