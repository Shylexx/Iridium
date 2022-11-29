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
    // Equivalent of check for multiple possible token types
    bool match(int count, tok::TokType types...);
    tok::Token consume(tok::TokType type, std::string errMessage);

    AST::Stmt declaration();
    AST::Stmt fnDeclaration();
    AST::Stmt statement();
    AST::Stmt returnStatement();
    AST::Stmt forStatement();
    AST::Stmt whileStatement();
    AST::Stmt blockStatement();
    AST::Stmt ifStatement();
    AST::Stmt exprStatement();
    AST::Stmt scopeStatement();

  };
} // namespace iridium
#endif
