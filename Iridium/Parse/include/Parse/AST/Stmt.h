#ifndef __IRIDIUM_STMT_AST_H__
#define __IRIDIUM_STMT_AST_H__

#include "Lex/Token.h"
#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/NodeType.h"
#include "Parse/Type/Type.h"
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {


class Stmt {
public:
  virtual ~Stmt() {}
  virtual void Accept(ASTVisitor* visitor) const = 0;
  NodeType node() { return nodeType; }
private:
  NodeType nodeType = NodeType::None;
};

class Err : public Stmt {
public:
  ~Err() override {}
  Err(std::string message) : m_Message(message) {}
  Err(std::string message, int line) : m_Message(message), m_SourceLine(line) {}
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitErrStmt(this);
  }
  std::string m_Message;
  int m_SourceLine = 0;
private:
  NodeType nodeType = NodeType::ErrorNode;
};

class ProtoStmt : public Stmt {
public:
  ~ProtoStmt() override {}
  ProtoStmt(const std::string& name,
      std::vector<std::pair<tok::Token, ty::Type>> params,
      ty::Type type = ty::Type::Ty_Void)
    : name(name), params(std::move(params)), retType(type) {}

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitProtoStmt(this);
  }

  std::string name;
  std::vector<std::pair<tok::Token, ty::Type>> params;
  ty::Type retType = ty::Type::Ty_Void;
private:
  NodeType nodeType = NodeType::FnProtoNode;
};

class FnStmt : public Stmt {
public:
  ~FnStmt() override {}
  FnStmt(std::unique_ptr<ProtoStmt> prototype,
         std::unique_ptr<Stmt> body)
      : Proto(std::move(prototype)), body(std::move(body)) {}

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitFnStmt(this);
  }

  std::unique_ptr<ProtoStmt> Proto;
  std::unique_ptr<Stmt> body;
private:
  NodeType nodeType = NodeType::FnDeclNode;
};

class VarDeclStmt : public Stmt {
public:
  ~VarDeclStmt() override {}
  VarDeclStmt(const std::string &Name, ty::Type ty, std::unique_ptr<AST::Expr>&& initializer = {}) : m_Name(Name), m_Initializer(std::move(initializer)), type(ty) {}
  
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitVarDeclStmt(this);
  }

  ty::Type type;
  std::string m_Name;
  std::unique_ptr<AST::Expr> m_Initializer;
private:
  NodeType nodeType = NodeType::VarDeclNode;
};

class GlobVarDeclStmt : public Stmt {
public:
  ~GlobVarDeclStmt() override {}
  GlobVarDeclStmt(const std::string &Name, ty::Type ty, std::unique_ptr<AST::Expr>&& initializer = {}) : m_Name(Name), m_Initializer(std::move(initializer)), type(ty) {}
  
  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitGlobVarDeclStmt(this);
  }

  ty::Type type;
  std::string m_Name;
  std::unique_ptr<AST::Expr> m_Initializer;
private:
  NodeType nodeType = NodeType::GlobVarDeclNode;
};

class IfStmt : public Stmt {
public:
  IfStmt(
      std::unique_ptr<Expr> Cond,
      std::unique_ptr<Stmt> Then,
      std::unique_ptr<Stmt> Else)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}
  ~IfStmt() override {}

  IfStmt(
      std::unique_ptr<Expr> Cond,
      std::unique_ptr<Stmt> Then)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(nullptr) {}

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitIfStmt(this);
  }

  std::unique_ptr<Expr> Cond;
  std::unique_ptr<Stmt> Then;
  std::unique_ptr<Stmt> Else;
};
class BlockStmt : public Stmt {
public:
  ~BlockStmt() override {}
  BlockStmt(std::vector<std::unique_ptr<Stmt>> body) : body(std::move(body))  {}
  BlockStmt() {};
  void Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBlockStmt(this);
  }

  std::vector<std::unique_ptr<Stmt>> body;
};

class ExprStmt : public Stmt {
public:
  ~ExprStmt() override {}
  ExprStmt(std::unique_ptr<AST::Expr> expr)
    : Expression(std::move(expr)) {
      if(Expression->nodeType == NodeType::ErrorNode) {
	nodeType = NodeType::ErrorNode;
      }
    }

  std::unique_ptr<AST::Expr> Expression;

  void Accept(ASTVisitor* visitor) const override {
	  visitor->VisitExprStmt(this);
  }

private:
  NodeType nodeType = NodeType::ExprStmtNode;
};
} // namespace AST
} // namespace iridium
#endif
