#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__

#include "Lex/TokType.h"
#include "Parse/AST/Stmt.h"
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace iridium {
namespace AST {

class Expr {
public:
  virtual ~Expr() = default;
  virtual llvm::Value* codegen() = 0; // Needs LLVM Linked!!! // TODO
};

class BinaryExpr : public Expr {
public:
  ~BinaryExpr() override {}
  BinaryExpr(tok::TokType oper, std::unique_ptr<Expr> LHS,
             std::unique_ptr<Expr> RHS)
      : op(oper), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  tok::TokType op;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
};

class IntExpr : public Expr {
public:
  IntExpr(int value) : Val(value) {}
  int Val;
};

class BoolExpr : public Expr {
public:
  BoolExpr(bool value) : Val(value) {}
  bool Val;
};

class BlockExpr : public Expr {
public:
  ~BlockExpr() override {}
  BlockExpr(std::vector<std::unique_ptr<Stmt>> body) : body(std::move(body)) {}

private:
  std::vector<std::unique_ptr<Stmt>> body;
};

class CallExpr : public Expr {
public:
  ~CallExpr() override {}
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : Callee(callee), Args(std::move(args)) {}

  std::string Callee;
  std::vector<std::unique_ptr<Expr>> Args;
};

} // namespace AST
} // namespace iridium
#endif
