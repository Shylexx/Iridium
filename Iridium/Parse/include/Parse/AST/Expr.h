#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__

#include "Lex/TokType.h"
#include "Parse/Type/Type.h"
#include "Parse/AST/ASTVisitor.h"
#include "Parse/Type/Context.h"
#include "Parse/AST/NodeType.h"
#include <cmath>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {

class Stmt;

enum class ExprType {
  None,
  Var,
  Return,
  Unary,
  Binary,
  Int,
  Float,
  Bool,
  Call,
  Logical,
  Assign,
  Err,
};


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
  virtual ty::Type tyCheck(ty::Context* context) const = 0;

  virtual const ExprType exprType() const { return ExprType::None; }
};

class UnaryExpr : public Expr {
public:
  ~UnaryExpr() override {}
  UnaryExpr(tok::TokType oper, std::unique_ptr<Expr> RHS, ty::Type type)
    : Op(oper), RHS(std::move(RHS)), Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitUnaryExpr(this);
  }

  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitUnaryExpr(this);
  }

  virtual const ExprType exprType() const override { return ExprType::Unary; }

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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitBinaryExpr(this);
  }

  tok::TokType op;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
  virtual const ExprType exprType() const override { return ExprType::Binary; }
};

class IntExpr : public Expr {
public:
  IntExpr(int value, ty::Type type) : Val(value), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitIntExpr(this);
  }
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitIntExpr(this);
  }
  int Val;
  virtual const ExprType exprType() const override { return ExprType::Int; }
};

class FloatExpr : public Expr {
public:
  FloatExpr(float value, ty::Type type) : Val(value), Expr(type) {}

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitFloatExpr(this);
  }
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitFloatExpr(this);
  }

  float Val;
  virtual const ExprType exprType() const override { return ExprType::Float; }
};

class BoolExpr : public Expr {
public:
  BoolExpr(bool value, ty::Type type) : Val(value), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBoolExpr(this);
  }
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitBoolExpr(this);
  }
  bool Val;
  virtual const ExprType exprType() const override { return ExprType::Bool; }
};


class CallExpr : public Expr {
public:
  ~CallExpr() override {}
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args, ty::Type type)
      : Callee(callee), Args(std::move(args)), Expr(type) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitCallExpr(this);
  }
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitCallExpr(this);
  }

  std::string Callee;
  std::vector<std::unique_ptr<Expr>> Args;
  virtual const ExprType exprType() const override { return ExprType::Call; }
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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitReturnExpr(this);
  }

  std::unique_ptr<Expr> Val;
  virtual const ExprType exprType() const override { return ExprType::Return; }
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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitVarExpr(this);
  }
  virtual const ExprType exprType() const override { return ExprType::Var; }

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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitAssignExpr(this);
  }
  virtual const ExprType exprType() const override { return ExprType::Assign; }
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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitLogicalExpr(this);
  }
  virtual const ExprType exprType() const override { return ExprType::Logical; }
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
  ty::Type tyCheck(ty::Context* context) const override {
    return context->VisitErrExpr(this);
  }

  NodeType nodeType = NodeType::ErrorNode;
  int Line;
  virtual const ExprType exprType() const override { return ExprType::Err; }
private:
  std::string Msg = "";

};


} // namespace AST
} // namespace iridium
#endif
