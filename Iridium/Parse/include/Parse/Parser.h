#ifndef __IRIDIUM_PARSER_H__
#define __IRIDIUM_PARSER_H__

#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include "Lex/Token.h"
#include "Parse/AST/Unit.h"
#include <string>
#include <vector>
#include <memory>

namespace iridium {
  class Parser {
  public:
    Parser();
    ~Parser();

    void ParseFile(const std::string& fileSource);
    void ParseTokenSet(const std::vector<tok::Token>& tokens);

    void printSyntaxErrs();
    void printLexedToks();
    AST::Unit m_CurUnit;
  private:
    int m_CurTok = 0;

    bool hasError = false;
    std::string errMsg = "";


    // Index of the current nested scope
    // 0 is equivalent to global scope
    int m_ScopeIndex = 0;

    std::unique_ptr<AST::Stmt> ParseStmt();

    std::unique_ptr<Lexer> m_Lexer;

    // Returns if we are at the end of the token set
    bool atEnd();
    // Returns the next token to be consumed
    tok::Token peek();
    // Looks at the token consumed last
    tok::Token previous();
    // Moves through the set of tokens
    tok::Token advance();
    // Checks the next token without consuming it and returns if it is correct
    bool check(tok::TokType type);
    // Equivalent of check but consumes the token
    bool match(tok::TokType type);
    // helper for adding an error node;
    std::unique_ptr<AST::Stmt> makeError(std::string errMsg);

    tok::Token consume(tok::TokType type, std::string errMessage);

    // Will consume any typename 
    tok::Token consumeTy(std::string errMessage);

    // Statements
    std::unique_ptr<AST::Stmt> statement();
    std::unique_ptr<AST::Stmt> declaration();
    // Items
    std::unique_ptr<AST::Stmt> fnDeclaration();
    std::unique_ptr<AST::Stmt> varDeclaration(tok::TokType type);

    std::unique_ptr<AST::Stmt> exprStatement();

    std::unique_ptr<AST::Expr> expression();
    std::unique_ptr<AST::Expr> paren();
    std::unique_ptr<AST::Expr> ternary();
    std::unique_ptr<AST::Expr> term();
    std::unique_ptr<AST::Expr> factor();
    std::unique_ptr<AST::Expr> unary();
    std::unique_ptr<AST::Expr> primary();
    std::unique_ptr<AST::Expr> identifier();


    std::unique_ptr<AST::Expr> returnExpr();
    std::unique_ptr<AST::Stmt> forStatement();
    std::unique_ptr<AST::Stmt> whileStatement();
    std::unique_ptr<AST::Stmt> blockStatement();
    std::unique_ptr<AST::Stmt> ifStatement();
    std::vector<std::unique_ptr<AST::Stmt>> blockExpr();

  };
} // namespace iridium
#endif
