#include "Parse/Parser.h"
#include "Parse/AST/Fn.h"
#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Lex/Token.h"
#include "Parse/AST/NodeType.h"
#include "Parse/AST/Expr.h"
#include "Parse/Type/Type.h"
#include <iostream>
#include <llvm/Support/TimeProfiler.h>
#include <memory>
#include <cstdarg>
#include <utility>

namespace iridium {
    Parser::Parser() {
      m_Lexer = std::make_unique<Lexer>();
    }
    Parser::~Parser() {}

    void Parser::ParseTokenSet(const std::vector<tok::Token>& tokens) {}

    bool Parser::ParseFile(const std::string& source) {
      m_Lexer->LexString(source);

      printLexedToks();

      if(!ResolveItems()) {
        std::cerr << "Failed to resolve file" << std::endl;
        return false;
      }
      std::cerr << "Successfully resolved file" << std::endl;

      // Reset curtok to 0 to begin parsing again
      m_CurTok = 0;
      while (!atEnd()) {
        m_CurUnit.add(declaration());
        if (m_CurUnit.error()) {
          m_CurUnit.errMessage();
          return false;
        }
      }

      return true;
    }

    bool Parser::ResolveItems() {

      while(!atEnd()) {
        if(match(tok::TokType::Fn)) {
          // if we error, return false to show the err
          std::unique_ptr<AST::FnProto> proto = fnProto();
          if(proto->error) {
            std::cerr << "Error parsing function prototype!" << std::endl;
            std::cerr << "Syntax Error on line [" << proto->line()
            << "]: " << proto->errMsg << std::endl;
              return false;
          }
          if(!m_CurUnit.addProto(std::move(proto))) {
            std::cerr << "Cannot Redefine Existing Function!" << std::endl;
            //m_CurUnit.protoErrMessage();
            return false;
          }
        }
        advance();
      }

      return true;
    }

    std::unique_ptr<AST::Stmt> Parser::declaration() {
      // The only top level declarations are items
      // (Functions are items)
      if (match(tok::TokType::Fn)) {
        return fnDefinition();
      }  
      // Only look for other statements if we are not in the global scope (can be a function body)
      if (m_ScopeIndex > 0) {
        if (match(tok::TokType::i64KW)) {
          return varDeclaration(tok::TokType::i64);
        }
        if (match(tok::TokType::i32KW)) {
          return varDeclaration(tok::TokType::i32);
        }
        if (match(tok::TokType::f64KW)) {
          return varDeclaration(tok::TokType::f64);
        }
        if (match(tok::TokType::f32KW)) {
          return varDeclaration(tok::TokType::f32);
        }
        if (match(tok::TokType::StringKW)) {
          return varDeclaration(tok::TokType::String);
        }
      //if(match(tok::TokType::Struct)) {
      //  return structDeclaration();
      //}

        return statement();
      }

      return std::make_unique<AST::Err>("Can only Declare Functions or Global Variables in Global scope!", currentLine());
    }

    std::unique_ptr<AST::Stmt> Parser::statement()  {
      if(match(tok::TokType::For)) {
        return forStatement();
      }



      if(match(tok::TokType::While)) {
        return whileStatement();
      }

      return exprStatement();
    }

    int Parser::currentLine() {
      return m_Lexer->curLine();
    }

    // Returns whether or not the next token is of the select type
    bool Parser::check(tok::TokType type) {
      if (atEnd()) {
        return false;
      }
      return peek().getTokType() == type;
    }

    // Consumes the type if the next matches, otherwise returns false
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
          || check(tok::TokType::i32KW)
          || check(tok::TokType::f64KW)
          || check(tok::TokType::f32KW)
          || check(tok::TokType::BoolKW)
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
      return std::move(std::make_unique<AST::Err>(errMsg, currentLine()));
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

      std::unique_ptr<AST::Expr> initializer;
      if(match(tok::TokType::Assignment)) {
        initializer = expression();
      }

      consume(tok::TokType::Semicolon, "Expected ';' after variable declaration!");
      if (hasError) {
        return makeError(errMsg);
      }

      if(m_ScopeIndex < 1) {
        if(!initializer.get()) {
          return std::make_unique<AST::Err>("Global Variables require an initializer!", currentLine());
        }
        std::cerr << "Parsed a global var decl of name " << name.getString() << std::endl;
        return std::make_unique<AST::GlobVarDeclStmt>(name.getString(), ty::from_keyword(type), std::move(initializer));
      }


      std::cout << "Parsed a Variable Declaration of name " << name.getString() << std::endl;
      return std::make_unique<AST::VarDeclStmt>(name.getString(), ty::from_keyword(type), std::move(initializer));
    }

    std::unique_ptr<AST::Stmt> Parser::fnProto() {
      std::string name = consume(tok::TokType::Identifier, "Expected function name!").getString();
      if (hasError) {
        return makeError(errMsg);
      }

      consume(tok::TokType::OpenParen, "Expected '(' after function identifier");

      if (hasError) {
        return makeError(errMsg);
      }

      std::vector<std::pair<tok::Token, ty::Type>> params;
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
          params.push_back(std::make_pair<tok::Token, ty::Type>(std::move(paramName), ty::from_tok(paramType.getTokType())));
        } while (match(tok::TokType::Comma));
      }

      consume(tok::TokType::CloseParen, "Expected ')' after function parameter(s)");
      if (hasError) {
        return makeError(errMsg);
      }

      ty::Type retType = ty::Type::Ty_Void;
      if(match(tok::TokType::Arrow)) {
        tok::TokType tokType = advance().getTokType();
        ty::Type maybeType = ty::from_keyword(tokType);
        if(maybeType != ty::Type::Ty_Err) {
          retType = maybeType;
        } else {
          std::cerr << "Expected typename after function return type specifier" << std::endl;
        }
      }

      std::cerr << "Parsed a function prototype: " << name << std::endl;
      return std::make_unique<AST::ProtoStmt>(std::move(name), std::move(params), retType);
    }

    std::unique_ptr<AST::Stmt> Parser::fnDefinition() {

      std::unique_ptr<AST::Stmt> prototype = fnProto();

      // if parsing proto doesnt get a prototype, return the error node
      if(!dynamic_cast<AST::ProtoStmt*>(prototype.get())) {
        return prototype;
        std::cerr << "Proto Stmt did not cast" << std::endl;
      }

      // make new unique ptr from abstract one and release old pointer
      // this is dodgy but if it works it works
      std::unique_ptr<AST::ProtoStmt> proto(static_cast<AST::ProtoStmt*>(prototype.release()));

      consume(tok::TokType::OpenBrace, "Expected '{' after function declaration");
      if (hasError) {
        return makeError(errMsg);
      }

      std::cerr << "Parsing body for : " << proto->name << std::endl;
      m_CurFunction = proto->name;

      std::unique_ptr<AST::Expr> body = blockExpr();

      m_CurFunction.clear();

      std::cout << "Parsed a function with name: " <<
        static_cast<AST::ProtoStmt*>(proto.get())->name << 
        " and arity of " << static_cast<AST::ProtoStmt*>(proto.get())->params.size() << std::endl;
      
      return std::make_unique<AST::FnStmt>(std::move(proto), std::move(body));
    }

    std::unique_ptr<AST::Expr> Parser::ifExpr() {
      auto condition = expression();
      if (!condition) {
        return std::make_unique<AST::ErrExpr>("Error parsing condition for if statement");
      }

      consume(tok::TokType::OpenBrace, "Expected '{' at start of if statement's 'then' block");

      auto then = blockExpr();
      if (!then) {
        return std::make_unique<AST::ErrExpr>("Error parsing 'then' block of if statement");
      }

      std::unique_ptr<AST::BlockExpr> elseBlock = std::make_unique<AST::BlockExpr>(ty::Type::Ty_Void);
      if(match(tok::TokType::Else)) {
        consume(tok::TokType::OpenBrace, "Expected open brace at start of else branch");
        std::unique_ptr<AST::Expr> elseBlock = blockExpr();
        if(!elseBlock) {
          return std::make_unique<AST::ErrExpr>("Error parsing 'else' block of if statement");
        }
        return std::make_unique<AST::IfExpr>(std::move(condition), std::move(then), std::move(elseBlock), ty::Type::Ty_Void);
      }
      return std::make_unique<AST::IfExpr>(std::move(condition), std::move(then), std::move(elseBlock), ty::Type::Ty_Void);
    }

    std::unique_ptr<AST::Expr> Parser::returnExpr() {
      if(check(tok::TokType::Semicolon)) {
        if(m_CurUnit.m_Functions[m_CurFunction].get()->retType == ty::Type::Ty_Void) {
          std::cerr << "Parsing empty return" << std::endl;
          consume(tok::TokType::Semicolon, "Expected ';' after return");
          return std::make_unique<AST::ReturnExpr>();
        }
        consume(tok::TokType::Semicolon, "Expected ';' after return");
        return std::make_unique<AST::ErrExpr>("Function must return value of matching type to declaration");
      } else {
        std::cerr << "Parsing return expr" << std::endl;
        std::unique_ptr<AST::Expr> expr = expression();
        if(m_CurUnit.m_Functions[m_CurFunction].get()->retType == expr->retType) {
          auto returnExpr = std::make_unique<AST::ReturnExpr>(std::move(expr), expr->retType);
          consume(tok::TokType::Semicolon, "Expected ';' after return expression");
          return std::move(returnExpr);
        }
        consume(tok::TokType::Semicolon, "Expected ';' after return expression");
        return std::make_unique<AST::ErrExpr>("Function must return value of matching type to declaration");
      }
    }

    std::unique_ptr<AST::Stmt> Parser::forStatement() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }

    std::unique_ptr<AST::Stmt> Parser::whileStatement() {

      return std::make_unique<AST::Err>("Unimplemented!");
    }


    std::unique_ptr<AST::Stmt> Parser::exprStatement() {
      std::unique_ptr<AST::Expr> expr;

      switch(advance().getTokType()) {
        case tok::TokType::Return: 
          expr = returnExpr();
          break;
        case tok::TokType::If:
          expr = ifExpr();
          break;
        default:
          expr = expression();
          break;
      }
      
      return std::make_unique<AST::ExprStmt>(std::move(expr));
    }

    std::unique_ptr<AST::Expr> Parser::expression() {
      return assignment();
    }

    std::unique_ptr<AST::Expr> Parser::assignment() {
      std::unique_ptr<AST::Expr> expr = orExpr();

      if(peek().getTokType() == tok::TokType::Assignment) {
        //consume the '='
        advance();

        // find the value to be assigned to
        std::unique_ptr<AST::Expr> value = assignment();

        if(m_Lexer->get(m_CurTok - 2).getTokType() == tok::TokType::Identifier) {
          std::string target = static_cast<AST::VarExpr*>(expr.get())->Iden;
          ty::Type type = static_cast<AST::VarExpr*>(expr.get())->retType;
          return std::make_unique<AST::AssignExpr>(target, std::move(value), type);
        } else {
          return std::make_unique<AST::ErrExpr>("Cannot assign a value to that target", currentLine());
        }
      }
      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::orExpr() {
      std::unique_ptr<AST::Expr> expr = andExpr();

      while(match(tok::TokType::Logical_OR)) {
        std::unique_ptr<AST::Expr> rhs = andExpr();
        expr = std::make_unique<AST::LogicalExpr>(AST::LogicOp::OP_OR, std::move(expr), std::move(rhs), ty::Type::Ty_Bool);
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::andExpr() {
      std::unique_ptr<AST::Expr> expr = equality();

      while(match(tok::TokType::Logical_AND)) {
        std::unique_ptr<AST::Expr> rhs = equality();
        expr = std::make_unique<AST::LogicalExpr>(AST::LogicOp::OP_AND, std::move(expr), std::move(rhs), ty::Type::Ty_Bool);
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::equality() {
      std::unique_ptr<AST::Expr> expr = comparison();

      while(match(tok::TokType::Equality) || match(tok::TokType::NotEquality)) {
        tok::TokType op = previous().getTokType();
        std::unique_ptr<AST::Expr> rhs = comparison();
        expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr), std::move(rhs), ty::Type::Ty_Bool);
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::comparison() {
      std::unique_ptr<AST::Expr> expr = term();

      while(
          match(tok::TokType::GreaterThan)
          || match(tok::TokType::GreaterOrEqual)
          || match(tok::TokType::LessThan)
          || match(tok::TokType::LessOrEqual)
          ) {
        tok::TokType op = previous().getTokType();
        std::unique_ptr<AST::Expr> rhs = term();
        if(expr->retType == rhs->retType) {
          expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr), std::move(rhs), ty::Type::Ty_Bool);
        } else {
          std::cerr << "binary comparison must compare two expressions of matching type" << std::endl;
          return std::make_unique<AST::ErrExpr>("binary comparison must compare two expressions of matching type", currentLine());
        }
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::paren() {
      //std::cerr << "Parsed a paren grouping" << std::endl;

      std::unique_ptr<AST::Expr> V = expression();
      if (!V) {
        std::cerr << "Couldn't parse paren subexpr" << std::endl;
        return nullptr;
      }

      if (peek().getTokType() != tok::TokType::CloseParen) {
        std::cerr << "Couldn't find ending paren" << std::endl;
        return std::make_unique<AST::ErrExpr>("Missing Close Parenthesis after Expression!", currentLine());
      }

      // consume close bracket 
      advance();

      return V;
    }

    std::unique_ptr<AST::Expr> Parser::ternary() {
      auto expr = term();

      if(match(tok::TokType::QuestionMark)) {
        auto thenBranch = ternary();
        consume(tok::TokType::Colon, "Expected colon after ternary condition.");
        auto elseBranch = ternary();
        // return std::make_unique<AST::TernaryExpr>(expr, thenBranch, elseBranch);
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::term() {
      std::unique_ptr<AST::Expr> expr = factor();

      while(match(tok::TokType::Minus) || match(tok::TokType::Plus)) {
        std::cerr << "Parsed a term" << std::endl;
        tok::TokType op = previous().getTokType();
        std::unique_ptr<AST::Expr> rhs = factor();
        if(expr->retType == rhs->retType) {
          expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr), std::move(rhs), expr->retType);
        } else {
          std::cerr << "Mismatched types in binary expression" << std::endl;
          return std::make_unique<AST::ErrExpr>("Mismatched types in binary expression", currentLine());
        }
      }
      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::factor() {
      std::unique_ptr<AST::Expr> expr = unary();

      while(match(tok::TokType::Slash) || match(tok::TokType::Asterisk)) {
        std::cerr << "Parsed a factor" << std::endl;
        tok::TokType op = previous().getTokType();
        std::unique_ptr<AST::Expr> rhs = unary();
        if(expr->retType == rhs->retType) {
          expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr), std::move(rhs), expr->retType);
        } else {
          std::cerr << "Mismatched types in binary expression" << std::endl;
          return std::make_unique<AST::ErrExpr>("Mismatched types in binary expression", currentLine());
        }
      }

      return expr;
    }

    std::unique_ptr<AST::Expr> Parser::unary() {
      // unary expr when ready :)
      if (match(tok::TokType::Minus) || match(tok::TokType::Exclaim)) {
        std::cerr << "Parsed a Unary expr" << std::endl;
        tok::TokType op = previous().getTokType();
        std::unique_ptr<AST::Expr> expr = unary();
        ty::Type type;
        if(op == tok::TokType::Exclaim) {
          type = ty::Type::Ty_Bool;
        } else { // if not boolean NOT, use the existing type
          type = expr->retType;
        }
        return std::make_unique<AST::UnaryExpr>(op, std::move(expr), type);
      }

      return primary();
    }

    std::unique_ptr<AST::Expr> Parser::primary() {
      tok::Token primary = advance();
      switch(primary.getTokType()) {
        case tok::TokType::i64:
          std::cerr << "Parsed an integer" << std::endl;
          return std::make_unique<AST::IntExpr>(primary.geti64(), ty::Type::Ty_i64);
        case tok::TokType::i32:
          std::cerr << "Parsed an integer" << std::endl;
          return std::make_unique<AST::IntExpr>(primary.geti64(), ty::Type::Ty_i32);
        case tok::TokType::f64:
          //std::cerr << "Parsed a float" << std::endl;
          return std::make_unique<AST::FloatExpr>(primary.getf64(), ty::Type::Ty_f64);        
        case tok::TokType::Identifier:
          return identifier();
        case tok::TokType::OpenParen:
          return paren();
        default:
          return std::make_unique<AST::ErrExpr>("Parser expected an expression", currentLine());
      }
    }


    std::unique_ptr<AST::Expr> Parser::identifier() {
      //std::cerr << "Parsed an identifier" << std::endl;
      std::string name = previous().getString();

      // If no bracket, its a var ref
      if (peek().getTokType() != tok::TokType::OpenParen) {

        auto result = m_CurUnit.m_Vars.find(name);
        if(result == m_CurUnit.m_Vars.end()) {
          std::cerr << "Use of an undefined variable '" << name << "'" << std::endl;
          return std::make_unique<AST::ErrExpr>("Undefined variable" + name, currentLine());
        }
        return std::make_unique<AST::VarExpr>(name, result->second->type);
      }
      // otherwise, Function Call
      // consume the '('
      advance();
      std::vector<std::unique_ptr<AST::Expr>> args;
      if (peek().getTokType() != tok::TokType::CloseParen) {
        while(1) {
          if(auto Arg = expression())
            args.push_back(std::move(Arg));
          else
            return nullptr;

          if(peek().getTokType() == tok::TokType::CloseParen)
            break;

          if(peek().getTokType() != tok::TokType::Comma)
            return nullptr;

          // consume the comma
          advance();
        }
      }

      // consume closing ')'
      advance();

      auto result = m_CurUnit.m_Functions.find(name);
      if (result == m_CurUnit.m_Functions.end()) {
        std::cerr << "Cannot call functions that do not exist!" << std::endl;
        return std::make_unique<AST::ErrExpr>("Function doesnt exist", currentLine());
      }
      AST::ProtoStmt* function = result->second.get();
      if(args.size() != function->params.size()) {
        std::cerr << "Incorrect amount of parameters passed to function of name '" << function->name << "'" << std::endl;
        return std::make_unique<AST::ErrExpr>("Incorrect param count", currentLine());
      }
      return std::make_unique<AST::CallExpr>(name, std::move(args), m_CurUnit.m_Functions[name]->retType);
    }

    std::unique_ptr<AST::Expr> Parser::blockExpr() {
      std::vector<std::unique_ptr<AST::Stmt>> stmts;
      m_ScopeIndex += 1;
      
      while(!check(tok::TokType::CloseBrace) && !atEnd()) {
        stmts.push_back(std::move(declaration()));
      }

      //std::cerr << "token at end of block was: " << tok::TokToString(m_Lexer->get(m_CurTok)) << std::endl;
      consume(tok::TokType::CloseBrace, "Expected '}' at end of block!");
      if (hasError) {
        stmts.clear();
        stmts.push_back(std::move(makeError(errMsg)));
      }
      // Return to the higher scope
      m_ScopeIndex -= 1;
      return std::make_unique<AST::BlockExpr>(std::move(stmts), ty::Type::Ty_Void);
    }
    
    void Parser::printSyntaxErrs() {
      m_Lexer->syntaxErrorsToCerr();
    }

    void Parser::printLexedToks() {
      std::cout << m_Lexer->DumpTokenTypes() << std::endl;
    }
} // namespace iridium
