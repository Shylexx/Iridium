#include "Parse/Parser.h"
#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Lex/Token.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/NodeType.h"
#include "Parse/AST/Stmt.h"
#include "Parse/Type/Type.h"
#include <cstdarg>
#include <iostream>
#include <llvm/Support/TimeProfiler.h>
#include <memory>
#include <utility>

namespace iridium {
Parser::Parser() { m_Lexer = std::make_unique<Lexer>(); }
Parser::~Parser() {}

void Parser::ParseTokenSet(const std::vector<tok::Token> &tokens) {}

bool Parser::ParseFile(const std::string &source) {
  m_Lexer->LexString(source);

  printLexedToks();

  if (!ResolveItems()) {
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

  if(m_CurUnit.m_Items.size() == 0) {
    std::cerr << "No item declarations in source code!\n";
    return false;
  }

  std::optional<std::vector<std::string>> tyErrors = m_CurUnit.m_Context.CheckTypes();

  if(tyErrors.has_value()) {
    for(auto& err : tyErrors.value()) {
      std::cerr << "Type Error: " << err << std::endl;
      return false;
    }
  }

  return true;
}

bool Parser::ResolveItems() {

  while (!atEnd()) {
    if( match(tok::TokType::Struct)) {
      std::unique_ptr<AST::Stmt> structDef = structDefinition();
      m_CurUnit.m_Context.addStruct(std::move(structDef));
    }
    if (match(tok::TokType::Fn)) {
      // if we error, return false to show the err
      std::unique_ptr<AST::Stmt> proto = fnProto();
      if(hasError()) {
        std::cerr << "Error parsing function prototype!" << std::endl;
        return false;
      }
      std::unique_ptr<AST::ProtoStmt> casted(
          static_cast<AST::ProtoStmt *>(proto.release()));
      if (!m_CurUnit.addProto(std::move(casted))) {
        std::cerr << "Cannot Redefine Existing Function!" << std::endl;
        // m_CurUnit.protoErrMessage();
        return false;
      }
    } else if(match(tok::TokType::Extern)) {
      // consume the 'extern'
      advance();
      std::unique_ptr<AST::Stmt> proto = fnProto();
      if(hasError()) {
        std::cerr << "Error parsing function prototype!" << std::endl;
        return false;
      }
      std::unique_ptr<AST::ProtoStmt> casted(
          static_cast<AST::ProtoStmt *>(proto.release()));
      if (!m_CurUnit.addProto(std::move(casted))) {
        std::cerr << "Cannot Redefine Existing Function!" << std::endl;
        // m_CurUnit.protoErrMessage();
        return false;
      }
    }
    advance();
  }

  return true;
}

std::unique_ptr<AST::Stmt> Parser::declaration() {
  // skip struct decls
  if(match(tok::TokType::Struct)) {
    /*
    while(previous().getTokType() != tok::TokType::CloseBrace) {
      advance();
    }
    */
    return structDefinition();
  }
  // The only top level declarations are items
  // (Functions are items)
  if (match(tok::TokType::Fn)) {
    return fnDefinition();
  } 
  if (match(tok::TokType::Extern)) {
    if(!match(tok::TokType::Fn)) {
      std::cerr << "Only functions can be declared as extern" << std::endl;
      return std::make_unique<AST::Err>("Only functions can be declared as extern");
    }
    std::unique_ptr<AST::Stmt> prototype = fnProto();
    if (hasError()) {
      return std::make_unique<AST::Err>("Error parsing function prototype");
    }
    std::cerr << "Parsed extern fn with name: " << static_cast<AST::ProtoStmt*>(prototype.get())->name << std::endl;
    // consume the semicolon
    advance();
    return prototype;
  }
  // Only look for other statements if we are not in the global scope (can be a
  // function body)
  if (m_ScopeIndex > 0) {
    if(match(tok::TokType::i8KW)) {
      return varDeclaration(ty::tyType::Ty_i8);
    }
    if (match(tok::TokType::i64KW)) {
      return varDeclaration(ty::tyType::Ty_i64);
    }
    if (match(tok::TokType::i32KW)) {
      return varDeclaration(ty::tyType::Ty_i32);
    }
    if (match(tok::TokType::f64KW)) {
      return varDeclaration(ty::tyType::Ty_f64);
    }
    if (match(tok::TokType::f32KW)) {
      return varDeclaration(ty::tyType::Ty_f32);
    }
    if (match(tok::TokType::BoolKW)) {
      return varDeclaration(ty::tyType::Ty_Bool);
    }
    if(peek().getTokType() == tok::TokType::Identifier) {
      // check if iden matches a struct type
      if(m_CurUnit.m_Context.m_Environment[peek().getString()].size() > 0) {
        // if so, its a var decl of the struct
        auto tok = advance();
        return varDeclaration(ty::Type(ty::tyType::Ty_Struct,false, tok.getString()));
      }
    }
    /*
    if (match(tok::TokType::StringKW)) {
      return varDeclaration(ty::Type::Ty_Void);
    }
    */
    // if(match(tok::TokType::Struct)) {
    //   return structDeclaration();
    // }

    return statement();
  }

  std::cerr << tok::TokToString(peek()) << "\n";
  return std::make_unique<AST::Err>(
      "Can only Declare Functions or Global Variables in Global scope!",
      currentLine());
}

std::unique_ptr<AST::Stmt> Parser::statement() {
  if (match(tok::TokType::For)) {
    return forStatement();
  }

  if (match(tok::TokType::While)) {
    return whileStatement();
  }

  if (match(tok::TokType::If)) {
    return ifStmt();
  }

  return exprStatement();
}

int Parser::currentLine() { return m_Lexer->curLine(); }

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
    m_HasError = true;
    errMsg = errMessage;
    return tok::Token(0, tok::TokType::ParseError, errMessage);
  }
}

tok::Token Parser::consumeTy(std::string errMessage) {
  if (check(tok::TokType::i64KW) 
      || check(tok::TokType::i32KW) 
      || check(tok::TokType::f64KW) 
      || check(tok::TokType::f32KW) 
      || check(tok::TokType::BoolKW)) {
    return advance();
  } else {
    std::cerr << "Parse Error: " << errMessage << std::endl;
    m_HasError = true;
    errMsg = errMessage;
    return tok::Token(0, tok::TokType::ParseError, errMessage);
  }
}

std::unique_ptr<AST::Stmt> Parser::makeError(std::string errMsg) {
  m_HasError = true;
  return std::move(std::make_unique<AST::Err>(errMsg, currentLine()));
}

std::unique_ptr<AST::Expr> Parser::makeErrExpr(std::string errMsg) {
  m_HasError = true;
  return std::move(std::make_unique<AST::ErrExpr>(errMsg, currentLine()));
}

tok::Token Parser::advance() {
  if (!atEnd()) {
    m_CurTok++;
  }
  return previous();
}

tok::Token Parser::previous() { return m_Lexer->get(m_CurTok - 1); }

tok::Token Parser::peek() { return m_Lexer->get(m_CurTok); }

bool Parser::atEnd() { return peek().getTokType() == tok::TokType::EndOfFile; }

std::unique_ptr<AST::Stmt> Parser::varDeclaration(ty::Type type) {
  std::cerr << "Var declaration" << std::endl;
  std::cerr << "Struct field keyword is: " << ty::to_string(type) << std::endl;
  tok::Token name =
      consume(tok::TokType::Identifier, "Expected Identifier For Variable!");
  if (hasError()) {
    return makeError(errMsg);
  }

  if(m_CurUnit.m_Vars[m_CurFunction].contains(name.getString())) {
    return makeError("Redefinition of variable with name " + name.getString());
  }

  std::unique_ptr<AST::Expr> initializer;
  if (match(tok::TokType::Assignment)) {
    initializer = expression();
  }

  consume(tok::TokType::Semicolon, "Expected ';' after variable declaration!");
  if (hasError()) {
    return makeError(errMsg);
  }

  
  if (m_ScopeIndex < 1) {
    if (!initializer.get()) {
      return std::make_unique<AST::Err>(
          "Global Variables require an initializer!", currentLine());
    }
    std::cerr << "Parsed a global var decl of name " << name.getString()
              << std::endl;
    return std::make_unique<AST::GlobVarDeclStmt>(name.getString(), type,
                                                  std::move(initializer));
  }
  

  std::cout << "Parsed a Variable Declaration of name " << name.getString()
            << std::endl;
  if(!inStructDecl()) {
    m_CurUnit.m_Vars[m_CurFunction][name.getString()] = type;
  }
  return std::make_unique<AST::VarDeclStmt>(name.getString(), type,
                                            std::move(initializer));
}

std::unique_ptr<AST::Stmt> Parser::fnProto() {
  std::string name =
      consume(tok::TokType::Identifier, "Expected function name!").getString();
  if (hasError()) {
    return makeError(errMsg);
  }

  m_CurFunction = name;

  consume(tok::TokType::OpenParen, "Expected '(' after function identifier");

  if (hasError()) {
    std::cerr << errMsg << std::endl;
    return makeError(errMsg);
  }

  std::vector<std::pair<tok::Token, ty::Type>> params;
  // If there are params, add them to the list.
  if (!check(tok::TokType::CloseParen)) {
    do {
      tok::Token paramName =
          consume(tok::TokType::Identifier, "Expected parameter name!");
      if (hasError()) {
        std::cerr << errMsg << std::endl;
        return makeError(errMsg);
      }
      consume(tok::TokType::Colon,
              "Expected ':' between parameter name and type!");
      if (hasError()) {
        std::cerr << errMsg << std::endl;
        return makeError(errMsg);
      }
      tok::Token paramType = consumeTy("Expected typename!");
      if (hasError()) {
        std::cerr << errMsg << std::endl;
        return makeError(errMsg);
      }
      params.push_back(std::make_pair<tok::Token, ty::Type>(
          std::move(paramName), ty::from_keyword(paramType.getTokType())));
    } while (match(tok::TokType::Comma));
  }

  for(auto& pair : params) {
    m_CurUnit.m_Vars[m_CurFunction][pair.first.getString()] = pair.second;
  }

  consume(tok::TokType::CloseParen, "Expected ')' after function parameter(s)");
  if (hasError()) {
    return makeError(errMsg);
  }

  ty::Type retType = ty::tyType::Ty_Void;
  if (match(tok::TokType::Arrow)) {
    tok::TokType tokType = advance().getTokType();
    ty::Type maybeType = ty::from_keyword(tokType);
    if (maybeType.type() != ty::tyType::Ty_Err) {
      retType = maybeType;
    } else {
      std::cerr << "Expected typename after function return type specifier"
                << std::endl;
    }
  }

  std::cerr << "Parsed a function prototype: " << name << std::endl;
  return std::make_unique<AST::ProtoStmt>(std::move(name), std::move(params),
                                          retType);
}

std::unique_ptr<AST::Stmt> Parser::fnDefinition() {

  std::unique_ptr<AST::Stmt> prototype = fnProto();
  if (hasError()) {
    return std::make_unique<AST::Err>("Error parsing function prototype");
  }

  // if parsing proto doesnt get a prototype, return the error node
  /*
  if (!dynamic_cast<AST::ProtoStmt *>(prototype.get())) {
    return prototype;
    std::cerr << "Proto Stmt did not cast" << std::endl;
  }
  */

  // make new unique ptr from abstract one and release old pointer
  // this is dodgy but if it works it works
  std::unique_ptr<AST::ProtoStmt> proto(
      static_cast<AST::ProtoStmt *>(prototype.release()));

  consume(tok::TokType::OpenBrace, "Expected '{' after function declaration");
  if (hasError()) {
    return makeError(errMsg);
  }

  //cur func is set in proto parsing so that parameters are accurately registered as variable bindings
  std::cerr << "Parsing body for : " << proto->name << std::endl;
  std::cerr << "Cur func is set to: " << m_CurFunction << std::endl;
  std::cerr << "map size is: " << m_CurUnit.m_Vars[m_CurFunction].size() << "\n";

  std::unique_ptr<AST::Stmt> body = blockStmt();

  m_CurFunction.clear();

  std::cout << "Parsed a function with name: "
            << static_cast<AST::ProtoStmt *>(proto.get())->name
            << " return type of: "
            << ty::to_string(
                   static_cast<AST::ProtoStmt *>(proto.get())->retType)
            << " and arity of "
            << static_cast<AST::ProtoStmt *>(proto.get())->params.size()
            << std::endl;

  return std::make_unique<AST::FnStmt>(std::move(proto), std::move(body));
}

std::unique_ptr<AST::Stmt> Parser::structDefinition() {
  auto nameTok = consume(tok::TokType::Identifier, "Expected Struct name");
  m_InStruct = true;

  consume(tok::TokType::OpenBrace, "Expected '{' after struct name");
  m_ScopeIndex += 1;

  std::vector<std::unique_ptr<AST::Stmt>> fields;
  // parse the fields
  while(peek().getTokType() != tok::TokType::CloseBrace) {

    fields.push_back(varDecl());

    if(hasError()) {
      return makeError("Error parsing struct fields");
    }
  }

  consume(tok::TokType::CloseBrace, "Expected '}' after struct fields");
  m_ScopeIndex -= 1;

  for(auto& field : fields) {
    std::cerr << "Struct field type at parse time: " << ty::to_string(static_cast<AST::VarDeclStmt*>(field.get())->type) << std::endl;
  }

  m_InStruct = true;
  return std::make_unique<AST::StructDefStmt>(nameTok.getString(), std::move(fields));
}

std::unique_ptr<AST::Stmt> Parser::varDecl() {
    if(match(tok::TokType::i8KW)) {
      return varDeclaration(ty::tyType::Ty_i8);
    }
    if (match(tok::TokType::i64KW)) {
      return varDeclaration(ty::tyType::Ty_i64);
    }
    if (match(tok::TokType::i32KW)) {
      return varDeclaration(ty::tyType::Ty_i32);
    }
    if (match(tok::TokType::f64KW)) {
      return varDeclaration(ty::tyType::Ty_f64);
    }
    if (match(tok::TokType::f32KW)) {
      return varDeclaration(ty::tyType::Ty_f32);
    }
    if (match(tok::TokType::BoolKW)) {
      return varDeclaration(ty::tyType::Ty_Bool);
    }
    if (match(tok::TokType::Identifier)) {
      return varDeclaration(ty::tyType::Ty_Struct);
    }
    return makeError("Expected typename to declare a variable");
}

std::unique_ptr<AST::Stmt> Parser::ifStmt() {
  auto condition = expression();
  if (!condition) {
    return std::make_unique<AST::Err>(
        "Error parsing condition for if statement");
  }

  auto token = consume(tok::TokType::OpenBrace,
          "Expected '{' at start of if statement's 'then' block");

  std::cerr << "tok at start of then block" << tok::TokToString(token) << std::endl;

  std::cerr << "parsing then block of ifstmt" << std::endl;
  auto then = blockStmt();
  if (!then) {
    return std::make_unique<AST::Err>(
        "Error parsing 'then' block of if statement");
  }

  std::unique_ptr<AST::BlockStmt> elseBlock =
      std::make_unique<AST::BlockStmt>();
  if (match(tok::TokType::Else)) {
    consume(tok::TokType::OpenBrace,
            "Expected open brace at start of else branch");
    std::unique_ptr<AST::Stmt> elseBlock = blockStmt();
    if (!elseBlock) {
      return std::make_unique<AST::Err>(
          "Error parsing 'else' block of if statement");
    }
    return std::make_unique<AST::IfStmt>(std::move(condition), std::move(then),
                                         std::move(elseBlock));
  }
  return std::make_unique<AST::IfStmt>(std::move(condition), std::move(then),
                                       std::move(elseBlock));
}

std::unique_ptr<AST::Expr> Parser::returnExpr() {
  if (check(tok::TokType::Semicolon)) {
    if (m_CurUnit.m_Functions[m_CurFunction].get()->retType ==
        ty::Type(ty::tyType::Ty_Void)) {
      std::cerr << "Parsing empty return" << std::endl;
      consume(tok::TokType::Semicolon, "Expected ';' after return");
      return std::make_unique<AST::ReturnExpr>();
    }
    consume(tok::TokType::Semicolon, "Expected ';' after return");
    return std::make_unique<AST::ErrExpr>(
        "Function must return value of matching type to declaration");
  } else {
    std::cerr << "Parsing return expr" << std::endl;
    std::unique_ptr<AST::Expr> expr = expression();
    if (m_CurUnit.m_Functions[m_CurFunction].get()->retType == expr->retType) {
      auto returnExpr =
          std::make_unique<AST::ReturnExpr>(std::move(expr), expr->retType);
      consume(tok::TokType::Semicolon, "Expected ';' after return expression");
      return std::move(returnExpr);
    }
    consume(tok::TokType::Semicolon, "Expected ';' after return expression");
    return std::make_unique<AST::ErrExpr>(
        "Function must return value of matching type to declaration");
  }
}

std::unique_ptr<AST::Stmt> Parser::forStatement() {

  return std::make_unique<AST::Err>("Unimplemented!");
}

std::unique_ptr<AST::Stmt> Parser::whileStatement() {

  auto condition = expression();
  if (!condition) {
    return std::make_unique<AST::Err>(
        "Error parsing condition for while statement");
  }

  auto token = consume(tok::TokType::OpenBrace,
          "Expected '{' at start of while statement's body block");

  std::cerr << "tok at start of while body block" << tok::TokToString(token) << std::endl;

  std::cerr << "parsing then block of while stmt" << std::endl;
  auto body = blockStmt();
  if (!body) {
    return std::make_unique<AST::Err>(
        "Error parsing body of while statement");
  }

  return std::make_unique<AST::WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<AST::Stmt> Parser::exprStatement() {
  std::unique_ptr<AST::Expr> expr;

  switch (peek().getTokType()) {
  case tok::TokType::Return:
    // consume return keyword
    advance();
    expr = returnExpr();
    break;
  default:
    expr = expression();
    consume(tok::TokType::Semicolon, "Expected ';' at end of expression");
    break;
  }


  return std::make_unique<AST::ExprStmt>(std::move(expr));
}

std::unique_ptr<AST::Expr> Parser::expression() { return assignment(); }

std::unique_ptr<AST::Expr> Parser::assignment() {
  // expr is the identifier
  std::unique_ptr<AST::Expr> expr = orExpr();

  if (peek().getTokType() == tok::TokType::Assignment) {
    std::cerr << "Parsing assignment" << std::endl;
    // consume the '='
    advance();

    // find the value to assign to the identifier
    std::unique_ptr<AST::Expr> value = assignment();

    if(hasError()) {
      return makeErrExpr("Variable Initializer Error");
    }

    std::cerr << "expr iden: " << static_cast<AST::VarExpr *>(expr.get())->Iden << std::endl;
    if (expr->exprType() == AST::ExprType::Var) {
      std::string target = static_cast<AST::VarExpr *>(expr.get())->Iden;
      ty::Type type = static_cast<AST::VarExpr *>(expr.get())->retType;
      std::cerr << "assign expr" << "\n";
      return std::make_unique<AST::AssignExpr>(target, std::move(value), type);
    } else {
      std::cerr << "prev tok is: " << tok::TokToString(m_Lexer->get(m_CurTok - 2)) << std::endl;
      return std::make_unique<AST::ErrExpr>(
          "Cannot assign a value to that target", currentLine());
    }
  }
  return expr;
}

std::unique_ptr<AST::Expr> Parser::orExpr() {
  std::unique_ptr<AST::Expr> expr = andExpr();

  while (match(tok::TokType::Logical_OR)) {
    std::unique_ptr<AST::Expr> rhs = andExpr();
    expr =
        std::make_unique<AST::LogicalExpr>(AST::LogicOp::OP_OR, std::move(expr),
                                           std::move(rhs), ty::Type(ty::tyType::Ty_Bool));
  }

  return expr;
}

std::unique_ptr<AST::Expr> Parser::andExpr() {
  std::unique_ptr<AST::Expr> expr = equality();

  while (match(tok::TokType::Logical_AND)) {
    std::unique_ptr<AST::Expr> rhs = equality();
    expr = std::make_unique<AST::LogicalExpr>(AST::LogicOp::OP_AND,
                                              std::move(expr), std::move(rhs),
                                              ty::Type(ty::tyType::Ty_Bool));
  }

  return expr;
}

std::unique_ptr<AST::Expr> Parser::equality() {
  std::unique_ptr<AST::Expr> expr = comparison();

  while (match(tok::TokType::Equality) || match(tok::TokType::NotEquality)) {
    tok::TokType op = previous().getTokType();
    std::unique_ptr<AST::Expr> rhs = comparison();
    expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr),
                                             std::move(rhs), ty::Type(ty::tyType::Ty_Bool));
  }

  return expr;
}

std::unique_ptr<AST::Expr> Parser::comparison() {
  std::unique_ptr<AST::Expr> expr = term();

  while (match(tok::TokType::GreaterThan) ||
         match(tok::TokType::GreaterOrEqual) || match(tok::TokType::LessThan) ||
         match(tok::TokType::LessOrEqual)) {
    tok::TokType op = previous().getTokType();
    std::unique_ptr<AST::Expr> rhs = term();
    if (expr->retType == rhs->retType) {
      expr = std::make_unique<AST::BinaryExpr>(
          op, std::move(expr), std::move(rhs), ty::Type(ty::tyType::Ty_Bool));
    } else {
      std::cerr << ty::to_string(expr->retType);
      std::cerr << ty::to_string(rhs->retType);
      std::cerr << "Mismatched types in binary expression" << std::endl;
      return std::make_unique<AST::ErrExpr>(
          "Mismatched types in binary expression", currentLine());
    }
  }

  return expr;
}

std::unique_ptr<AST::Expr> Parser::paren() {
  // std::cerr << "Parsed a paren grouping" << std::endl;

  std::unique_ptr<AST::Expr> V = expression();
  if (!V) {
    std::cerr << "Couldn't parse paren subexpr" << std::endl;
    return nullptr;
  }

  if (peek().getTokType() != tok::TokType::CloseParen) {
    std::cerr << "Couldn't find ending paren" << std::endl;
    return std::make_unique<AST::ErrExpr>(
        "Missing Close Parenthesis after Expression!", currentLine());
  }

  // consume close bracket
  advance();

  return V;
}

std::unique_ptr<AST::Expr> Parser::ternary() {
  auto expr = term();

  if (match(tok::TokType::QuestionMark)) {
    auto thenBranch = ternary();
    consume(tok::TokType::Colon, "Expected colon after ternary condition.");
    auto elseBranch = ternary();
    // return std::make_unique<AST::TernaryExpr>(expr, thenBranch, elseBranch);
  }

  return expr;
}

std::unique_ptr<AST::Expr> Parser::term() {
  std::unique_ptr<AST::Expr> expr = factor();

  while (match(tok::TokType::Minus) || match(tok::TokType::Plus)) {
    std::cerr << "Parsed a term" << std::endl;
    tok::TokType op = previous().getTokType();
    std::unique_ptr<AST::Expr> rhs = factor();
    if (expr->retType == rhs->retType) {
      expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr),
                                               std::move(rhs), expr->retType);
    } else {
      std::cerr << "Mismatched types in binary expression" << std::endl;
      return std::make_unique<AST::ErrExpr>(
          "Mismatched types in binary expression", currentLine());
    }
  }
  return expr;
}

std::unique_ptr<AST::Expr> Parser::factor() {
  std::unique_ptr<AST::Expr> expr = unary();

  while (match(tok::TokType::Slash) || match(tok::TokType::Asterisk)) {
    std::cerr << "Parsed a factor" << std::endl;
    tok::TokType op = previous().getTokType();
    std::unique_ptr<AST::Expr> rhs = unary();
    if (expr->retType == rhs->retType) {
      expr = std::make_unique<AST::BinaryExpr>(op, std::move(expr),
                                               std::move(rhs), expr->retType);
    } else {
      std::cerr << "Mismatched types in binary expression" << std::endl;
      return std::make_unique<AST::ErrExpr>(
          "Mismatched types in binary expression", currentLine());
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
    ty::Type type = expr->retType;
    if (op == tok::TokType::Exclaim) {
      type = ty::Type(ty::tyType::Ty_Bool);
    } else { // if not boolean NOT, use the existing type
    }
    return std::make_unique<AST::UnaryExpr>(op, std::move(expr), type);
  }

  return primary();
}

std::unique_ptr<AST::Expr> Parser::primary() {
  tok::Token primary = advance();
  switch (primary.getTokType()) {
  case tok::TokType::Integer:
    std::cerr << "Parsed an integer" << std::endl;
    return std::make_unique<AST::IntExpr>(primary.geti64(), ty::Type(ty::tyType::Ty_i32));
  case tok::TokType::Float:
    // std::cerr << "Parsed a float" << std::endl;
    return std::make_unique<AST::FloatExpr>(primary.getf64(), ty::Type(ty::tyType::Ty_f32));
  case tok::TokType::Identifier:
    return identifier();
  case tok::TokType::OpenParen:
    return paren();
  default:
    std::cerr << "No primary found:" << tok::TokToString(primary) << std::endl;
    return std::make_unique<AST::ErrExpr>("Parser expected an expression on line: " + std::to_string(currentLine()),
                                          currentLine());
  }
}

std::unique_ptr<AST::Expr> Parser::identifier() {
  // std::cerr << "Parsed an identifier" << std::endl;
  std::string name = previous().getString();

  // If no bracket, its a var ref
  if (peek().getTokType() != tok::TokType::OpenParen) {

    
    auto result = m_CurUnit.m_Vars[m_CurFunction].find(name);
    if (result == m_CurUnit.m_Vars[m_CurFunction].end()) {
      std::cerr << "Use of an undefined variable '" << name << "'" << std::endl;
      return std::make_unique<AST::ErrExpr>("Undefined variable" + name,
                                            currentLine());
    }
    
    std::cerr << "Parsed a variable ref: " << name << std::endl;
    return std::make_unique<AST::VarExpr>(name, result->second);
  }
  // otherwise, Function Call or struct getter
  // consume the '(' or '.'
  //while(true) {
    tok::Token tok = advance();
      std::vector<std::unique_ptr<AST::Expr>> args;
      if (peek().getTokType() != tok::TokType::CloseParen) {
        while (1) {
          if (auto Arg = expression())
            args.push_back(std::move(Arg));
          else
            return nullptr;

          if (peek().getTokType() == tok::TokType::CloseParen)
            break;

          if (peek().getTokType() != tok::TokType::Comma)
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
        return std::make_unique<AST::ErrExpr>("Function doesnt exist",
                                              currentLine());
      }
      AST::ProtoStmt *function = result->second.get();
      if (args.size() != function->params.size()) {
        std::cerr << "Incorrect amount of parameters passed to function of name '"
                  << function->name << "'" << std::endl;
        return std::make_unique<AST::ErrExpr>("Incorrect param count",
                                              currentLine());
      }
      return std::make_unique<AST::CallExpr>(name, std::move(args), 
          m_CurUnit.m_Functions[name]->retType);
    /*else if (tok.getTokType() == tok::TokType::Period) {
      // parse struct getter
      tok::Token field = consume(tok::TokType::Identifier, "Expected struct field name after '.'");

      //
      return std::make_unique<AST::FieldGetExpr>();
    }
  }
  */
}

std::unique_ptr<AST::Stmt> Parser::blockStmt() {
  std::cerr << "started block parsing" << std::endl;
  std::vector<std::unique_ptr<AST::Stmt>> stmts;
  m_ScopeIndex += 1;

  std::cerr << "first tok in block: " << tok::TokToString(m_Lexer->get(m_CurTok)) << std::endl;

  while (!check(tok::TokType::CloseBrace) && !atEnd()) {
    stmts.push_back(std::move(declaration()));
  }

  // std::cerr << "token at end of block was: " <<
  // tok::TokToString(m_Lexer->get(m_CurTok)) << std::endl;
  consume(tok::TokType::CloseBrace, "Expected '}' at end of block!");
  if (hasError()) {
    stmts.clear();
    stmts.push_back(std::move(makeError(errMsg)));
  }
  // Return to the higher scope
  m_ScopeIndex -= 1;
  std::cerr << "finished block parsing" << std::endl;
  return std::make_unique<AST::BlockStmt>(std::move(stmts));
}

void Parser::printSyntaxErrs() { m_Lexer->syntaxErrorsToCerr(); }

void Parser::printLexedToks() {
  std::cout << m_Lexer->DumpTokenTypes() << std::endl;
}
} // namespace iridium
