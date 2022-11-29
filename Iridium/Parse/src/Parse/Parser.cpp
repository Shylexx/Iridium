#include "Parse/Parser.h"
#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Parse/AST/Stmt.h"
#include <cstdarg>

namespace iridium {
    Parser::Parser() {
      m_Lexer = std::make_unique<Lexer>();
    }
    Parser::~Parser() {}

    void Parser::ParseTokenSet(const std::vector<tok::Token>& tokens) {}

    void Parser::ParseFile(const std::string& source) {
      m_Lexer->LexString(source);

      std::vector<AST::StmtAST> Stmts;
      while (atEnd()) {
        Stmts.emplace_back(declaration());
      }
    }

    AST::StmtAST Parser::declaration() {
      if(match(1, tok::TokType::Fn)) {
        return fnDeclaration();
      }
      //if(match(tok::TokType::Struct)) {
      //  return structDeclaration();
      //}
      if(match(1, tok::TokType::i64)) {
      }
      if(match(1, tok::TokType::f64)) {
      }
      if(match(1, tok::TokType::String)) {
      }

      return statement();
    }

    AST::StmtAST Parser::statement()  {
      if(match(1, tok::TokType::For)) {
        return forStatement();
      }

      if(match(1, tok::TokType::If)) {
        return ifStatement();
      }

      if(match(1, tok::TokType::Return)) {
        return returnStatement();
      }

      if(match(1, tok::TokType::While)) {
        return whileStatement();
      }

      if(match(1, tok::TokType::OpenBrace)) {
        return scopeStatement();
      }

      return exprStatement();
    }

    bool Parser::check(tok::TokType type) {
      if (atEnd()) {
        return false;
      }
      return peek().getTokType() == type;
    }

    bool Parser::match(int count, tok::TokType types...) {
      va_list args;
      va_start(args, types);

      for(int i = 0; i < count; i++) {
          if(check(va_arg(args, tok::TokType))) {
            va_end(args);
            return true; 
          }
      }

      va_end(args);
      return false;
    }

    tok::Token Parser::consume(tok::TokType type, std::string errMessage) {
      if (check(type)) {
        return advance();
      } else {
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
} // namespace iridium
