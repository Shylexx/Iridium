#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__

#include "Lex/TokType.h"
#include "Parse/Type/Type.h"
#include "Parse/AST/ASTVisitor.h"
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {

class Stmt;

class Expr {
public:
  virtual ~Expr() = default;
  // All expressions return something, if not specified, its void.
  ty::Type retType = ty::Type::Ty_Void;
  virtual llvm::Value* Accept(ASTVisitor* visitor) const = 0;
};

class BinaryExpr : public Expr {
public:
  ~BinaryExpr() override {}
  BinaryExpr(tok::TokType oper, std::unique_ptr<Expr> LHS,
             std::unique_ptr<Expr> RHS)
      : op(oper), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
     return visitor->VisitBinaryExpr(this);
  }

  tok::TokType op;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
};

class IntExpr : public Expr {
public:
  IntExpr(int value) : Val(value) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitIntExpr(this);
  }
  int Val;
};

class BoolExpr : public Expr {
public:
  BoolExpr(bool value) : Val(value) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBoolExpr(this);
  }
  bool Val;
};

class BlockExpr : public Expr {
public:
  ~BlockExpr() override {}
  BlockExpr(std::vector<std::unique_ptr<Stmt>> body) : body(std::move(body)) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitBlockExpr(this);
  }

private:
  std::vector<std::unique_ptr<Stmt>> body;
};

class CallExpr : public Expr {
public:
  ~CallExpr() override {}
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : Callee(callee), Args(std::move(args)) {}
  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitCallExpr(this);
  }

  std::string Callee;
  std::vector<std::unique_ptr<Expr>> Args;
};

class ReturnExpr : public Expr {
public:
  ~ReturnExpr() override {}
  ReturnExpr(std::unique_ptr<Expr> value)
    : Val(std::move(value)) {}

  ReturnExpr() = default;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitReturnExpr(this);
  }

  std::unique_ptr<Expr> Val;
};

class VarExpr : public Expr {
public:
  ~VarExpr() override {}

  VarExpr(const std::string& identifier)
    : Iden(identifier) {}

  std::string Iden;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitVarExpr(this);
  }

};

class ErrExpr : public Expr {
public:
  ~ErrExpr() override {}

  ErrExpr() = default;

  llvm::Value* Accept(ASTVisitor* visitor) const override {
    return visitor->VisitErrExpr(this);
  }

};


} // namespace AST
} // namespace iridium
#endif
