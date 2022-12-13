#include "Parse/Parser.h"
#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include <iostream>
#include <cstdarg>

namespace iridium {
    Parser::Parser() {
      m_Lexer = std::make_unique<Lexer>();
    }
    Parser::~Parser() {}

    void Parser::ParseTokenSet(const std::vector<tok::Token>& tokens) {}

    void Parser::ParseFile(const std::string& source) {
      m_Lexer->LexString(source);

      while (!atEnd()) {
        m_CurUnit.add(declaration());
        if (m_CurUnit.error()) {
          m_CurUnit.errMessage();
        }
      }
    }

    std::unique_ptr<AST::Stmt> Parser::declaration() {
      // The only top level declarations are variables
      // (Functions are also variables)
      if (match(tok::TokType::Fn)) {
        return fnDeclaration();
      }
      //if(match(tok::TokType::Struct)) {
      //  return structDeclaration();
      //}
      if (match(tok::TokType::i64KW)) {
        return varDeclaration(tok::TokType::i64);
      }
      if (match(tok::TokType::f64KW)) {
        return varDeclaration(tok::TokType::f64);
      }
      if (match(tok::TokType::StringKW)) {
        return varDeclaration(tok::TokType::String);
      }

      // Only look for other statements if we are not in the global scope (can be a function body)
      if (m_ScopeIndex > 0) {
        return statement();
      }

      return std::make_unique<AST::Err>("Can only Declare Functions or Variables in Global scope!");
    }

    std::unique_ptr<AST::Stmt> Parser::statement()  {
      if(match(tok::TokType::For)) {
        return forStatement();
      }

      if(match(tok::TokType::If)) {
        return ifStatement();
      }

      if(match(tok::TokType::Return)) {
        return returnStatement();
      }

      if(match(tok::TokType::While)) {
        return whileStatement();
      }

      if(match(tok::TokType::OpenBrace)) {
        return blockExpr();
      }

      return exprStatement();
    }

    bool Parser::check(tok::TokType type) {
      if (atEnd()) {
        return false;
      }
      return peek().getTokType() == type;
    }

    bool Parser::match(tok::TokType type) {
      if (!check(type)) {
        return false;
      }
      advance();
      return true;
    }


    tok::Token Parser::consume(tok::TokType type, std::string errMessage) {
      if (check(type)) {
        return advance();
      } else {
        std::cerr << "Parse Error: " << errMessage << std::endl;
        m_CurUnit.add(std::move(std::make_unique<AST::Err>("Unexpected Token Type!")));
        return tok::Token(0, tok::TokType::ParseError, errMessage);
      }
    }

    tok::Token Parser::advance() {
      if (!atEnd()) {
        m_CurTok++;
      }
      return previous();
    }

    tok::Token Parser::previous() {
      return m_Lexer->get(m_CurTok - 1);
    }

    tok::Token Parser::peek() {
      return m_Lexer->get(m_CurTok);
    }

    bool Parser::atEnd() {
      return peek().getTokType() == tok::TokType::EndOfFile;
    }
    

    std::unique_ptr<AST::Stmt> Parser::varDeclaration(tok::TokType type) {
      tok::Token name = consume(tok::TokType::Identifier, "Expected Identifier For Variable!");

      consume(tok::TokType::Semicolon, "Expected ';' after variable declaration!");

      std::cout << "Parsed a Variable Declaration of name " << name.getString() << std::endl;
      return std::make_unique<AST::VarDeclStmt>(name.getString());
    }

    std::unique_ptr<AST::Stmt> Parser::fnDeclaration() {

      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::returnStatement() {

      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::forStatement() {

      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::whileStatement() {

      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::ifStatement() {

      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::exprStatement() {
      
      return std::make_unique<AST::IfStmt>();
    }

    std::unique_ptr<AST::Stmt> Parser::blockExpr() {

      return std::make_unique<AST::IfStmt>();
    }
    
    void Parser::printSyntaxErrs() {
      m_Lexer->syntaxErrorsToCerr();
    }

    void Parser::printLexedToks() {
      std::cout << m_Lexer->DumpTokenTypes() << std::endl;
    }
} // namespace iridium
