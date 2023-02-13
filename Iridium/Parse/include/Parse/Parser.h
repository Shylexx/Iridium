#ifndef __IRIDIUM_PARSER_H__
#define __IRIDIUM_PARSER_H__

#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Stmt.h"
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

    // Parses the file into an AST.
    // Returns true on success, false on fail
    bool ParseFile(const std::string& fileSource);

    // Collect all function signatures into the Unit prior to parsing the content itself
    bool ResolveItems();
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

    std::string m_CurFunction;

    // return the current source code line from the lexer;
    int currentLine();
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
    std::unique_ptr<AST::Stmt> fnProto();
    std::unique_ptr<AST::Stmt> fnDefinition();
    std::unique_ptr<AST::Stmt> varDeclaration(tok::TokType type);

    std::unique_ptr<AST::Stmt> exprStatement();

    std::unique_ptr<AST::Expr> expression();
    std::unique_ptr<AST::Expr> paren();
    std::unique_ptr<AST::Expr> comparison(); 
    std::unique_ptr<AST::Expr> equality(); 
    std::unique_ptr<AST::Expr> assignment();
    std::unique_ptr<AST::Expr> orExpr();
    std::unique_ptr<AST::Expr> andExpr();
    std::unique_ptr<AST::Expr> ternary(); // TODO?
    std::unique_ptr<AST::Expr> term();
    std::unique_ptr<AST::Expr> factor();
    std::unique_ptr<AST::Expr> unary(); // TODO
    std::unique_ptr<AST::Expr> primary();
    // parses function calls and variable names
    std::unique_ptr<AST::Expr> identifier();
    // parses if expressions
    std::unique_ptr<AST::Expr> ifExpr();
    // parses blocks
    std::unique_ptr<AST::Expr> blockExpr();


    std::unique_ptr<AST::Expr> returnExpr();
    std::unique_ptr<AST::Stmt> forStatement();
    std::unique_ptr<AST::Stmt> whileStatement();


    // Types
    // std::unique_ptr<AST::Type> parseTypeName();

  };
} // namespace iridium
#endif
