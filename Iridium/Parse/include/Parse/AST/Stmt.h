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

/*
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
*/
} // namespace AST
} // namespace iridium
#endif
