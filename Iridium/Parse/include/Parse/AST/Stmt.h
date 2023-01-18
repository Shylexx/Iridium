#ifndef __IRIDIUM_STMT_AST_H__
#define __IRIDIUM_STMT_AST_H__

#include "Lex/Token.h"
#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Expr.h"
#include "Parse/Type/Type.h"
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {

enum class NodeType {
  None,
  ExprStmtNode,
  ErrorNode,
  FnDeclNode,
  VarDeclNode,
  IfStmtNode,
  BinExprNode,
};

class Stmt {
public:
  virtual ~Stmt() {}
  NodeType nodeType = NodeType::None;
  virtual void Accept(ASTVisitor* visitor) const = 0;
};

class Err : public Stmt {
public:
  ~Err() override {}
  Err(std::string message) : m_Message(message) {}
  Err(std::string message, int line) : m_Message(message), m_SourceLine(line) {}
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitErrStmt(this);
  }
  NodeType nodeType = NodeType::ErrorNode;
  std::string m_Message;
  int m_SourceLine = 0;
};

class FnStmt : public Stmt {
public:
  ~FnStmt() override {}
  FnStmt(const std::string &name,
         std::vector<std::pair<tok::Token, ty::Type>> params,
         std::vector<std::unique_ptr<Stmt>> body)
      : name(name), params(std::move(params)), body(std::move(body)) {}

  FnStmt(const std::string &name,
         std::vector<std::pair<tok::Token, ty::Type>> params,
	 ty::Type type,
         std::vector<std::unique_ptr<Stmt>> body)
      : name(name), params(std::move(params)), retType(type), body(std::move(body)) {}

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitFnStmt(this);
  }

  NodeType nodeType = NodeType::FnDeclNode;
  std::string name;
  std::vector<std::pair<tok::Token, ty::Type>> params;
  ty::Type retType = ty::Type::Ty_Void;
  std::vector<std::unique_ptr<Stmt>> body;
};

class VarDeclStmt : public Stmt {
public:
  ~VarDeclStmt() override {}
  VarDeclStmt(const std::string &Name) : m_Name(Name) {}
  
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitVarDeclStmt(this);
  }

  NodeType nodeType = NodeType::VarDeclNode;
  std::string m_Name;
};

class IfStmt : public Stmt {
public:
  IfStmt() {}
  ~IfStmt() override {}
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitIfStmt(this);
  }

  NodeType nodeType = NodeType::IfStmtNode;
};

class ExprStmt : public Stmt {
public:
  ~ExprStmt() override {}
  ExprStmt(std::unique_ptr<AST::Expr> expr)
    : Expression(std::move(expr)) {}

  std::unique_ptr<AST::Expr> Expression;

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitExprStmt(this);
  }

  NodeType nodeType = NodeType::ExprStmtNode;
};
} // namespace AST
} // namespace iridium
#endif
