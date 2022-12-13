#include "Parse/Parser.h"
#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Lex/Token.h"
#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include <iostream>
#include <memory>
#include <cstdarg>
#include <utility>

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
          break;
        }
      }
    }


    std::unique_ptr<AST::Stmt> Parser::declaration() {
      // The only top level declarations are items
      // (Functions are items)
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
        return returnExpr();
      }

      if(match(tok::TokType::While)) {
        return whileStatement();
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
        hasError = true;
        errMsg = errMessage;
        return tok::Token(0, tok::TokType::ParseError, errMessage);
      }
    }

    tok::Token Parser::consumeTy(std::string errMessage) {
      if (
          check(tok::TokType::StringKW)
          || check(tok::TokType::i64KW)
          || check(tok::TokType::f64KW)
         ) {
        return advance();
      } else {
        std::cerr << "Parse Error: " << errMessage << std::endl;
        hasError = true;
        errMsg = errMessage;
        return tok::Token(0, tok::TokType::ParseError, errMessage);
      }
    }

    std::unique_ptr<AST::Stmt> Parser::makeError(std::string errMsg) {
      return std::move(std::make_unique<AST::Err>(errMsg));
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
      if (hasError) {
        return makeError(errMsg);
      }

      consume(tok::TokType::Semicolon, "Expected ';' after variable declaration!");
      if (hasError) {
        return makeError(errMsg);
      }

      std::cout << "Parsed a Variable Declaration of name " << name.getString() << std::endl;
      return std::make_unique<AST::VarDeclStmt>(name.getString());
    }

    std::unique_ptr<AST::Stmt> Parser::fnDeclaration() {

      std::string name = consume(tok::TokType::Identifier, "Expected function name!").getString();
      if (hasError) {
        return makeError(errMsg);
      }

      consume(tok::TokType::OpenParen, "Expected '(' after function identifier");

      if (hasError) {
        return makeError(errMsg);
      }

      std::vector<std::pair<tok::Token, tok::Token>> params;
      // If there are params, add them to the list.
      if(!check(tok::TokType::CloseParen)) {
        do {
          tok::Token paramName = consume(tok::TokType::Identifier, "Expected parameter name!");
          if (hasError) {
            return makeError(errMsg);
          }
          consume(tok::TokType::Colon, "Expected ':' between parameter name and type!");
          if (hasError) {
            return makeError(errMsg);
          }
          tok::Token paramType = consumeTy("Expected typename!");
          if (hasError) {
            return makeError(errMsg);
          }
          params.push_back(std::make_pair<tok::Token, tok::Token>(std::move(paramName), std::move(paramType)));
        } while (match(tok::TokType::Comma));
      }

      consume(tok::TokType::CloseParen, "Expected ')' after function parameter(s)");
      if (hasError) {
        return makeError(errMsg);
      }

      consume(tok::TokType::OpenBrace, "Expected '{' after function declaration");
      if (hasError) {
        return makeError(errMsg);
      }
      std::vector<std::unique_ptr<AST::Stmt>> body = blockExpr();

      std::cout << "Parsed a function with name: " << name << " and arity of " << params.size() << std::endl;
      return std::make_unique<AST::FnStmt>(name, std::move(params), std::move(body));
    }

    std::unique_ptr<AST::Stmt> Parser::returnExpr() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::unique_ptr<AST::Stmt> Parser::forStatement() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::unique_ptr<AST::Stmt> Parser::whileStatement() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::unique_ptr<AST::Stmt> Parser::ifStatement() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::unique_ptr<AST::Stmt> Parser::exprStatement() {
      
      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::vector<std::unique_ptr<AST::Stmt>> Parser::blockExpr() {
      std::vector<std::unique_ptr<AST::Stmt>> stmts;
      m_ScopeIndex += 1;
      
      while(!check(tok::TokType::CloseBrace) && !atEnd()) {
        stmts.push_back(std::move(declaration()));
      }

      consume(tok::TokType::CloseBrace, "Expected '}' at end of block!");
      if (hasError) {
        stmts.clear();
        stmts.push_back(std::move(makeError(errMsg)));
        return stmts;
      }
      // Return to the higher scope
      m_ScopeIndex -= 1;
      return std::move(stmts);
    }
    
    void Parser::printSyntaxErrs() {
      m_Lexer->syntaxErrorsToCerr();
    }

    void Parser::printLexedToks() {
      std::cout << m_Lexer->DumpTokenTypes() << std::endl;
    }
} // namespace iridium
