//
// Created by Saikat Chakraborty on 9/24/20.
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Lexer.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>

#include <stack>
#include <string>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory FindFunctionCategory("");

/*
 * This helper function prints out previous declaration of a variable.
 */
void printVarReDeclarationInformation(const std::string &varName,
                                      int currentDefLineNo,
                                      int initialDefLineNo) {
  std::cout << "Redefining variable : \"" << varName << "\" at line "
            << currentDefLineNo << " which is initially defined at line "
            << initialDefLineNo << std::endl;
}

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
public:
  explicit FunctionVisitor(ASTContext *_context, CompilerInstance &_compiler)
      : context(_context), compiler(_compiler) {}

  int getLineNumber(Stmt *stmt) {
    FullSourceLoc fullSourceLoc = context->getFullLoc(stmt->getBeginLoc());
    int ln;
    if (!fullSourceLoc.isValid())
      ln = -1;
    else
      ln = fullSourceLoc.getSpellingLineNumber();
    return ln;
  }

  std::string getSource(Stmt *node) {
    bool invalid;
    CharSourceRange range =
        CharSourceRange::getTokenRange(node->getBeginLoc(), node->getEndLoc());
    std::string tokens(Lexer::getSourceText(range, compiler.getSourceManager(),
                                            compiler.getLangOpts(), &invalid));
    if (!invalid) {
      return tokens;
    } else {
      return "";
    }
  }

  bool isRecursiveFunction(FunctionDecl *function, Stmt *stmt) {
    // TODO Implement this function.
    return false;
  }

  void analyzeVariableRedefinition(Stmt *functionBody) {
    // TODO Analyze whether there is a variable redefinition.
    // If you find
    //      a variable "k" being redefined at line n,
    //      which was initially defined at line m
    // You have to call
    // printVarReDeclarationInformation(k, n, m)
  }


  std::string formatFunctionCall(CallExpr *callExpr) {
    std::string formattedExpr = "";
    // TODO: implement the reformat call expression extra credit.
    return formattedExpr;
  }

  void analyzeCallExpressionReformat(Stmt *root) {
    // We are doing a DFS on the input AST.
    // Whenever we find an AST node of type "CallExpr",
    // We reformat corresponding sub-tree.
    std::stack<Stmt *> stack;
    stack.push(root);
    while (!stack.empty()) {
      Stmt *node = stack.top();
      stack.pop();
      if (isa<CallExpr>(node)) {
        // This node is "CallExpr" Node, we need to reformat this.
        CallExpr *expr = dyn_cast<CallExpr>(node);
        int lineNo = getLineNumber(node);
        std::string reformattedFunctionCall = formatFunctionCall(expr);
        std::cout << "==================================================\n";
        std::cout << "Function call found at line " << lineNo << "\n";
        std::cout << "Original Source\n" << getSource(node) << "\n";
        std::cout << "Formatted Source\n" << reformattedFunctionCall << "\n";
        std::cout << "==================================================\n";
      } else {
        // Since current node is not a CallExpr, let's visit the children.
        for (auto child : node->children()) {
          if (child != nullptr) {
            // We only want to visit the children which are not null
            stack.push(child);
          }
        }
      }
    }
  }

  bool VisitFunctionDecl(FunctionDecl *function) {
    std::string functionName = function->getNameInfo().getName().getAsString();
    if (!function->hasBody()) {
      // If the function does not have any body, we will not analyze it.
      return false;
    }
    Stmt *body = function->getBody();

    // Task 1
    if (isRecursiveFunction(function, body)) {
      std::cout << functionName << " - recursive\n";
    } else {
      std::cout << functionName << " - non-recursive\n";
    }

    // Task 2
    analyzeVariableRedefinition(body);

    // Extra Credit
    analyzeCallExpressionReformat(body);
    return true;
  }

  /*
   * This function is called when the visitor encounters a For statement in the
   * AST.
   */
  bool VisitForStmt(ForStmt *forStmt) {
    // We will give them this code for a reference code.
    assert(isa<ForStmt>(forStmt));
    std::cerr << "==================================================\n";
    std::cerr << "Found For Statement at line : " << getLineNumber(forStmt)
              << "\n";
    std::cerr << "==================================================\n";
    return true;
  }

private:
  ASTContext *context;
  CompilerInstance &compiler;
};

class FunctionVisitorConsumer : public clang::ASTConsumer {
public:
  explicit FunctionVisitorConsumer(ASTContext *context,
                                   CompilerInstance &compiler)
      : visitor(context, compiler) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
  }

private:
  FunctionVisitor visitor;
};

class FunctionVisitAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef inFile) {
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
