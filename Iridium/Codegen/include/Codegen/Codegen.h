#ifndef __IRIDIUM_CODEGEN_H__
#define __IRIDIUM_CODEGEN_H__

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <memory>

#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Stmt.h"
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

  static const bool OPTIMIZE = true;

  void GenUnit(AST::Unit& unit);
  void CreateSymbols(const AST::Unit& unit);
  void InitModuleAndFPM();
  void Optimize();
  void PrintIR();

  std::unordered_map<std::string, llvm::StructType*> m_Structs;
  // statements
  void VisitStructDefStmt(const AST::StructDefStmt *stmt) override;
  void VisitExprStmt(const AST::ExprStmt *stmt) override; // TODO
  void VisitProtoStmt(const AST::ProtoStmt *stmt) override; // TODO
  void VisitFnStmt(const AST::FnStmt *stmt) override; // TODO
  void VisitVarDeclStmt(const AST::VarDeclStmt *stmt) override; // TODO
  void VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt) override; // TODO
  void VisitIfStmt(const AST::IfStmt *stmt) override; // TODO
  void VisitWhileStmt(const AST::WhileStmt *stmt) override; // TODO
  void VisitErrStmt(const AST::Err* stmt) override; // TODO
  void VisitBlockStmt(const AST::BlockStmt *expr) override; // TODO

  // expressions
  llvm::Value *VisitIntExpr(const AST::IntExpr *expr) override;
  llvm::Value *VisitFloatExpr(const AST::FloatExpr *expr) override;
  llvm::Value* VisitBinaryExpr(const AST::BinaryExpr* expr) override;
  llvm::Value *VisitBoolExpr(const AST::BoolExpr *expr) override;
  llvm::Value *VisitCallExpr(const AST::CallExpr *expr) override;
  llvm::Value *VisitReturnExpr(const AST::ReturnExpr *expr) override;
  llvm::Value *VisitVarExpr(const AST::VarExpr *expr) override;
  llvm::Value *VisitStructFieldExpr(const AST::StructFieldExpr *expr) override;
  llvm::Value *VisitErrExpr(const AST::ErrExpr *expr) override;
  llvm::Value *VisitAssignExpr(const AST::AssignExpr *expr) override;
  llvm::Value* VisitLogicalExpr(const AST::LogicalExpr *expr) override;
  llvm::Value* VisitUnaryExpr(const AST::UnaryExpr *expr) override;

  llvm::Value* GenError(const char *Str) {
    std::cerr << "Codegen Error: " << Str << std::endl;
    return nullptr;
  }

  llvm::Module* module() const { return m_Module.get(); }

private:
  llvm::Type* from_Ty(ty::Type type);
  std::unique_ptr<llvm::LLVMContext> m_Context;
  std::unique_ptr<llvm::IRBuilder<>> m_Builder;
  std::unique_ptr<llvm::Module> m_Module;
  std::unique_ptr<llvm::legacy::FunctionPassManager> m_FPM;
  std::map<std::string, llvm::AllocaInst*> m_NamedValues;
  llvm::Function* m_CurFunc{};
  std::unordered_map<const AST::Stmt*, llvm::Function*> m_FuncMap;
};
} // namespace iridium

#endif
