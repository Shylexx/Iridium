#ifndef __IRIDIUM_TY_CONTEXT_H__
#define __IRIDIUM_TY_CONTEXT_H__

#include "Parse/AST/ASTVisitor.h"
#include "Parse/Type/Type.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

namespace iridium {
  namespace AST {
    class Unit;
  }
namespace ty {
  class Context {
  public:
    Context(AST::Unit* parentUnit);
    // if there are type errors, returns a vec of the error messages
    std::optional<std::vector<std::string>> CheckTypes(const std::vector<AST::Stmt>& ast);
    std::optional<std::vector<std::string>> CheckFn(
        const AST::FnStmt* fn
        );
    
    // statements
    ty::Type VisitExprStmt(const AST::ExprStmt *stmt); // TODO
    ty::Type VisitProtoStmt(const AST::ProtoStmt *stmt); // TODO
    ty::Type VisitFnStmt(const AST::FnStmt *stmt); // TODO
    ty::Type VisitVarDeclStmt(const AST::VarDeclStmt *stmt); // TODO
    ty::Type VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt); // TODO
    ty::Type VisitIfStmt(const AST::IfStmt *stmt); // TODO
    ty::Type VisitErrStmt(const AST::Err* stmt); // TODO
    ty::Type VisitBlockStmt(const AST::BlockStmt *expr); // TODO

    // expressions
    ty::Type VisitIntExpr(const AST::IntExpr *expr);
    ty::Type VisitFloatExpr(const AST::FloatExpr *expr);
    ty::Type VisitBinaryExpr(const AST::BinaryExpr* expr);
    ty::Type VisitBoolExpr(const AST::BoolExpr *expr);
    ty::Type VisitCallExpr(const AST::CallExpr *expr);
    ty::Type VisitReturnExpr(const AST::ReturnExpr *expr);
    ty::Type VisitVarExpr(const AST::VarExpr *expr);
    ty::Type VisitErrExpr(const AST::ErrExpr *expr);
    ty::Type VisitAssignExpr(const AST::AssignExpr *expr);
    ty::Type VisitLogicalExpr(const AST::LogicalExpr *expr);
    ty::Type VisitUnaryExpr(const AST::UnaryExpr *expr);

    ty::Type GenError(const char *Str) {
      std::cerr << "Type Error: " << Str << std::endl;
      return ty::Type::Ty_Err;
    }

  private:
    std::vector<std::string> m_CustomTypes;
    // The current set of bindings we are type checking with
    std::unordered_map<std::string, ty::Type> m_CurBindings;

    std::vector<std::string> m_TypeErrors{};
    AST::Unit* m_Unit;

    std::string m_CurFunc;
    AST::ProtoStmt* getCurProto();

    void tyError(const std::string& msg);
  };
}
}
#endif
