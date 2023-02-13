#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__

#include "Lex/TokType.h"
#include "Parse/Type/Type.h"
#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/NodeType.h"
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {

class Stmt;


class Expr {
protected:
  Expr(ty::Type type)
    : retType(type) {}

public:
  virtual ~Expr() = default;
  // All expressions return something, if not specified, its void.
  ty::Type retType;
  NodeType nodeType = NodeType::Expr;

  virtual llvm::Value* Accept(ASTVisitor* visitor) const = 0;
};

class UnaryExpr : public Expr {
public:
  ~UnaryExpr() override {}
  UnaryExpr(tok::TokType oper, std::unique_ptr<Expr> RHS, ty::Type type)
    : Op(oper), RHS(std::move(RHS)), Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitUnaryExpr(this);
  }

  tok::TokType Op;
  std::unique_ptr<Expr> RHS;
};

class BinaryExpr : public Expr {
public:
  ~BinaryExpr() override {}
  BinaryExpr(tok::TokType oper, std::unique_ptr<Expr> LHS,
             std::unique_ptr<Expr> RHS, ty::Type type)
      : op(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
     return visitor->VisitBinaryExpr(this);
  }

  tok::TokType op;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
};

class IntExpr : public Expr {
public:
  IntExpr(int value, ty::Type type) : Val(value), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitIntExpr(this);
  }
  int Val;
};

class FloatExpr : public Expr {
public:
  FloatExpr(float value, ty::Type type) : Val(value), Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitFloatExpr(this);
  }

  float Val;
};

class BoolExpr : public Expr {
public:
  BoolExpr(bool value, ty::Type type) : Val(value), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBoolExpr(this);
  }
  bool Val;
};

class BlockExpr : public Expr {
public:
  ~BlockExpr() override {}
  BlockExpr(std::vector<std::unique_ptr<Stmt>> body, ty::Type type) : body(std::move(body)), Expr(type) {}
  BlockExpr(ty::Type type) : Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBlockExpr(this);
  }

private:
  std::vector<std::unique_ptr<Stmt>> body;
};

class CallExpr : public Expr {
public:
  ~CallExpr() override {}
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args, ty::Type type)
      : Callee(callee), Args(std::move(args)), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitCallExpr(this);
  }

  std::string Callee;
  std::vector<std::unique_ptr<Expr>> Args;
};

class IfExpr : public Expr {
public:
  IfExpr(
      std::unique_ptr<Expr> Cond,
      std::unique_ptr<Expr> Then,
      std::unique_ptr<Expr> Else,
      ty::Type type = ty::Type::Ty_Void)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)), Expr(type) {}
  ~IfExpr() override {}

  IfExpr(
      std::unique_ptr<Expr> Cond,
      std::unique_ptr<Expr> Then,
      ty::Type type = ty::Type::Ty_Void)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(nullptr), Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
	  return visitor->VisitIfExpr(this);
  }

  std::unique_ptr<Expr> Cond;
  std::unique_ptr<Expr> Then;
  std::unique_ptr<Expr> Else;
};

class ReturnExpr : public Expr {
public:
  ~ReturnExpr() override {}
  ReturnExpr(std::unique_ptr<Expr> value, ty::Type type)
    : Val(std::move(value)), Expr(type) {}

  ReturnExpr(ty::Type type = ty::Type::Ty_Void) : Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitReturnExpr(this);
  }

  std::unique_ptr<Expr> Val;
};

class VarExpr : public Expr {
public:
  ~VarExpr() override {}

  VarExpr(const std::string& identifier, ty::Type type)
    : Iden(identifier), Expr(type) {}

  std::string Iden;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitVarExpr(this);
  }

};

class AssignExpr : public Expr {
public:
  ~AssignExpr() override {}

  AssignExpr(const std::string& name,
      std::unique_ptr<AST::Expr> value, ty::Type type)
    : Name(name), Val(std::move(value)), Expr(type) {}

  std::string Name = "";
  std::unique_ptr<AST::Expr> Val;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitAssignExpr(this);
  }
};

enum class LogicOp {
  OP_AND,
  OP_OR,
  OP_NOT,
};

class LogicalExpr : public Expr {
public:
  ~LogicalExpr() override {}

  LogicalExpr(AST::LogicOp op, std::unique_ptr<AST::Expr> left,
      std::unique_ptr<AST::Expr> right, ty::Type type)
      : Op(op), LHS(std::move(left)), RHS(std::move(right)), Expr(type) {}

  AST::LogicOp Op;
  std::unique_ptr<AST::Expr> LHS;
  std::unique_ptr<AST::Expr> RHS;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitLogicalExpr(this);
  }
};

class ErrExpr : public Expr {
public:
  ~ErrExpr() override {}

  ErrExpr(const std::string& errMsg, int sourceLine = 0) 
    : Msg(errMsg), Expr(ty::Type::Ty_Err), Line(sourceLine) {}

  const std::string& message() const { return Msg; }

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitErrExpr(this);
  }

  NodeType nodeType = NodeType::ErrorNode;
  int Line;
private:
  std::string Msg = "";

};


} // namespace AST
} // namespace iridium
#endif
