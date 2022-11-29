#ifndef __IRIDIUM_PARSER_H__
#define __IRIDIUM_PARSER_H__

#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include "Lex/Token.h"
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
  private:
    int m_CurTok = 0;

    std::unique_ptr<AST::StmtAST> ParseStmt();

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
    // Equivalent of check for multiple possible token types
    bool match(int count, tok::TokType types...);
    tok::Token consume(tok::TokType type, std::string errMessage);

    AST::StmtAST declaration();
    AST::StmtAST fnDeclaration();
    AST::StmtAST statement();
    AST::StmtAST returnStatement();
    AST::StmtAST forStatement();
    AST::StmtAST whileStatement();
    AST::StmtAST blockStatement();
    AST::StmtAST ifStatement();
    AST::StmtAST exprStatement();
    AST::StmtAST scopeStatement();

  };
} // namespace iridium
#endif
