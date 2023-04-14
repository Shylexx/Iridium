#include "Parse/Type/Context.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Unit.h"
#include "Parse/AST/Stmt.h"
#include "Parse/Type/Type.h"
#include <optional>

namespace iridium {
namespace ty {
  Context::Context(AST::Unit* unit) {
    m_Unit = unit;
  }

  std::optional<std::vector<std::string>> Context::CheckTypes(const std::vector<AST::Stmt> &ast) {

    // if we have errors, return them
    if(m_TypeErrors.size() > 0) {
      return std::make_optional<std::vector<std::string>>(m_TypeErrors);
    } else {
    return std::nullopt;
    }
  }

  std::optional<std::vector<std::string>> Context::CheckProto(
      const AST::ProtoStmt *fn
      ) {
    fn->tyCheck(this);  
    if(m_TypeErrors.size() > 0) {
      return m_TypeErrors;
    } else { return std::nullopt; 
    }
  }

  std::optional<std::vector<std::string>> Context::CheckFn(
      const AST::FnStmt *fn
      ) {
    std::cerr << "typechecking fn: " << fn->Proto->name << std::endl;
    m_CurFunc = fn->Proto->name;
    m_CurBindings = m_Unit->m_Vars;
    std::cerr << "segfaults while type checking" << std::endl;
    fn->tyCheck(this);
    
    std::cerr << "fn checking done" << std::endl;
    // if we have errors, return them
    if(m_TypeErrors.size() > 0) {
      return m_TypeErrors;
    } else {
    return std::nullopt;
    }
  }

  AST::ProtoStmt* Context::getCurProto() {
    return m_Unit->m_Functions.at(m_CurFunc).get();
  }

  void Context::tyError(const std::string& errMsg) {
    std::cerr << "Type Error: " << errMsg << std::endl;
    m_TypeErrors.push_back(errMsg);
  }

  // Type check statements
  ty::Type Context::VisitIfStmt(const AST::IfStmt* stmt) { return ty::Type::Ty_Void; }

  ty::Type Context::VisitWhileStmt(const AST::WhileStmt* stmt) { return ty::Type::Ty_Void; }
  
  ty::Type Context::VisitProtoStmt(const AST::ProtoStmt *stmt) { return ty::Type::Ty_Void; }

  ty::Type Context::VisitErrStmt(const AST::Err *stmt) { return ty::Type::Ty_Void; }

  ty::Type Context::VisitBlockStmt(const AST::BlockStmt *expr) {
    for(auto& stmt : expr->body) {
      std::cerr << "tyc block body stmt" << std::endl;
      stmt->tyCheck(this);
    }
    // block itself returns void, so should be as such if using a block in an expr
    return ty::Type::Ty_Void;
  }

  ty::Type Context::VisitFnStmt(const AST::FnStmt *stmt) {
    std::cerr << "start type checking the body!" << std::endl;
    // typecheck the body (should be a blockstmt)
    stmt->body->tyCheck(this);
    // a function definition itself has no type!
    return ty::Type::Ty_Void;
  }

  ty::Type Context::VisitVarDeclStmt(const AST::VarDeclStmt *stmt) {
    return stmt->m_Initializer->tyCheck(this);
  }

  ty::Type Context::VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt) {
    return stmt->m_Initializer->tyCheck(this);
  }

  ty::Type Context::VisitExprStmt(const AST::ExprStmt *stmt) {
    // the type of an expr statement is equal to the type of the expr
    return stmt->Expression->tyCheck(this);
  }

  // Type check expressions

  ty::Type Context::VisitBinaryExpr(const AST::BinaryExpr *expr) {
    std::cerr << "tyc lhs" << std::endl;
    ty::Type lType = expr->LHS->tyCheck(this);
    std::cerr << "tyc rhs" << std::endl;
    ty::Type rType = expr->RHS->tyCheck(this);
    // check if lType and rType are compatible with operand
    if(lType != rType) {
      tyError("Can't use binary expression with mismatched types");
      return ty::Type::Ty_Void;
    }
    return lType;
  }

  ty::Type Context::VisitUnaryExpr(const AST::UnaryExpr* expr) {
    // have to check if expr can be unaried with the operand
    return expr->retType;
  }

  ty::Type Context::VisitIntExpr(const AST::IntExpr *expr) {
    std::cerr << "tyc int expr" << std::endl; 
    return ty::Type::Ty_i32;
  }

  ty::Type Context::VisitFloatExpr(const AST::FloatExpr* expr) {
    return ty::Type::Ty_f32;
  }

  ty::Type Context::VisitBoolExpr(const AST::BoolExpr *expr) {
    return ty::Type::Ty_Bool;
  }

  ty::Type Context::VisitCallExpr(const AST::CallExpr* expr) {
    return m_Unit->m_Functions.at(expr->Callee)->retType;
  }

  ty::Type Context::VisitVarExpr(const AST::VarExpr *expr) {
    std::cerr << "tyc var with name: " << expr->Iden << " and decled type: " << ty::to_string(m_CurBindings.at(expr->Iden)) << std::endl;
    return m_CurBindings.at(expr->Iden);
  }

  ty::Type Context::VisitReturnExpr(const AST::ReturnExpr *expr) {
    ty::Type fnType = m_Unit->m_Functions.at(m_CurFunc)->retType;
    if(fnType != expr->Val->tyCheck(this)) {
      tyError("Tried to return value of incorrect type from function");
    }
    // return expr itself is void
    return ty::Type::Ty_Void;
  }

  ty::Type Context::VisitErrExpr(const AST::ErrExpr *expr) {
    return ty::Type::Ty_Err;
  }

  ty::Type Context::VisitAssignExpr(const AST::AssignExpr *expr) {
    std::cerr << "assign expr" << std::endl;
    return expr->Val->tyCheck(this);
  }

  ty::Type Context::VisitLogicalExpr(const AST::LogicalExpr *expr) {
    auto lType = expr->LHS->tyCheck(this);
    auto rType = expr->RHS->tyCheck(this);
    if(lType != ty::Type::Ty_Bool || rType != ty::Type::Ty_Bool) {
      tyError("Cannot compare non boolean expressions using logical operator");
      return ty::Type::Ty_Err;
    }

    return ty::Type::Ty_Bool;
  }

  /*
  // expressions
  llvm::Value *VisitReturnExpr(const AST::ReturnExpr *expr) override;
  llvm::Value *VisitErrExpr(const AST::ErrExpr *expr) override;
  llvm::Value *VisitAssignExpr(const AST::AssignExpr *expr) override;
  llvm::Value* VisitLogicalExpr(const AST::LogicalExpr *expr) override;
  */

}
}
