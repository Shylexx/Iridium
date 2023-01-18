#ifndef __IRIDIUM_AST_VISITOR__
#define __IRIDIUM_AST_VISITOR__

#include "llvm/IR/Value.h"
namespace iridium {
namespace AST {
  class ExprStmt;
  class FnStmt;
  class VarDeclStmt;
  class IfStmt;
  class Err;

  class IntExpr;
  class BoolExpr;
  class BinaryExpr;
  class BlockExpr;
  class CallExpr;
  class ReturnExpr;
  class ErrExpr;
  class VarExpr;
}


  class ASTVisitor {
  public:
    // statements
    virtual void VisitExprStmt(const AST::ExprStmt* stmt) = 0;
    virtual void VisitFnStmt(const AST::FnStmt* stmt) = 0;
    virtual void VisitVarDeclStmt(const AST::VarDeclStmt* stmt) = 0;
    virtual void VisitIfStmt(const AST::IfStmt* stmt) = 0;
    virtual void VisitErrStmt(const AST::Err* stmt) = 0;


    //expressions
    virtual llvm::Value* VisitIntExpr(const AST::IntExpr* expr) = 0;
    virtual llvm::Value* VisitBoolExpr(const AST::BoolExpr* expr) = 0;
    virtual llvm::Value* VisitBinaryExpr(const AST::BinaryExpr* expr) = 0;
    virtual llvm::Value* VisitBlockExpr(const AST::BlockExpr* expr) = 0;
    virtual llvm::Value* VisitCallExpr(const AST::CallExpr* expr) = 0;
    virtual llvm::Value* VisitReturnExpr(const AST::ReturnExpr* expr) = 0;
    virtual llvm::Value* VisitVarExpr(const AST::VarExpr* expr) = 0;
    virtual llvm::Value* VisitErrExpr(const AST::ErrExpr* expr) = 0;

    
  };
}
#endif
