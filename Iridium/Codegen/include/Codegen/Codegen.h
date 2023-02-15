#ifndef __IRIDIUM_CODEGEN_H__
#define __IRIDIUM_CODEGEN_H__

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <memory>

#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Unit.h"
#include "Parse/Type/Type.h"
#include <iostream>
#include <map>

// Code Generator Driver

namespace iridium {
class Codegen : public ASTVisitor {
public:
  Codegen();

  static const bool OPTIMIZE = false;

  void GenUnit(AST::Unit& unit);
  void InitModuleAndFPM();
  void PrintIR();
  // statements
  void VisitFnProto(const AST::FnProto *stmt) override; // TODO
  void VisitFnDef(const AST::FnDef *stmt) override; // TODO
  void VisitErrStmt(const AST::Err* stmt) override; // TODO

  // expressions
  llvm::Value *VisitIntExpr(const AST::IntExpr *expr) override;
  llvm::Value *VisitFloatExpr(const AST::FloatExpr *expr) override;
  llvm::Value* VisitBinaryExpr(const AST::BinaryExpr* expr) override;
  llvm::Value *VisitBoolExpr(const AST::BoolExpr *expr) override;
  llvm::Value *VisitBlockExpr(const AST::BlockExpr *expr) override; // TODO
  llvm::Value *VisitCallExpr(const AST::CallExpr *expr) override;
  llvm::Value* VisitIfExpr(const AST::IfExpr *expr) override; // TODO
  llvm::Value *VisitReturnExpr(const AST::ReturnExpr *expr) override;
  llvm::Value *VisitVarExpr(const AST::VarExpr *expr) override;
  llvm::Value *VisitErrExpr(const AST::ErrExpr *expr) override;
  llvm::Value *VisitAssignExpr(const AST::AssignExpr *expr) override;
  llvm::Value* VisitLogicalExpr(const AST::LogicalExpr *expr) override;
  llvm::Value* VisitUnaryExpr(const AST::UnaryExpr *expr) override;

  llvm::Value* GenError(const char *Str) {
    std::cerr << "Codegen Error: " << Str << std::endl;
    return nullptr;
  }

private:
  llvm::Type* from_Ty(ty::Type type);
  std::unique_ptr<llvm::LLVMContext> m_Context;
  std::unique_ptr<llvm::IRBuilder<>> m_Builder;
  std::unique_ptr<llvm::Module> m_Module;
  std::unique_ptr<llvm::legacy::FunctionPassManager> m_FPM;
  std::map<std::string, llvm::Value*> m_NamedValues;
};
} // namespace iridium

#endif
