#ifndef __IRIDIUM_CODEGEN_H__
#define __IRIDIUM_CODEGEN_H__

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Unit.h"
#include "Parse/Type/Type.h"

// Code Generator Driver

namespace iridium {
class Codegen : public ASTVisitor {
public:
  Codegen();

  void GenUnit(AST::Unit& unit);
  void PrintIR();
  // statements
  void VisitExprStmt(const AST::ExprStmt *stmt) override; // TODO
  void VisitFnStmt(const AST::FnStmt *stmt) override; // TODO
  void VisitVarDeclStmt(const AST::VarDeclStmt *stmt) override; // TODO
  void VisitIfStmt(const AST::IfStmt *stmt) override; // TODO
  void VisitErrStmt(const AST::Err* stmt) override; // TODO

  // expressions
  llvm::Value *VisitIntExpr(const AST::IntExpr *expr) override;
  llvm::Value* VisitBinaryExpr(const AST::BinaryExpr* expr) override;
  llvm::Value *VisitBoolExpr(const AST::BoolExpr *expr) override;
  llvm::Value *VisitBlockExpr(const AST::BlockExpr *expr) override; // TODO
  llvm::Value *VisitCallExpr(const AST::CallExpr *expr) override;
  llvm::Value *VisitReturnExpr(const AST::ReturnExpr *expr) override;
  llvm::Value *VisitVarExpr(const AST::VarExpr *expr) override;
  llvm::Value *VisitErrExpr(const AST::ErrExpr *expr) override;

private:
  llvm::Type* from_Ty(ty::Type type);
  std::unique_ptr<llvm::LLVMContext> m_Context;
  std::unique_ptr<llvm::IRBuilder<>> m_Builder;
  std::unique_ptr<llvm::Module> m_Module;
};
} // namespace iridium

#endif
