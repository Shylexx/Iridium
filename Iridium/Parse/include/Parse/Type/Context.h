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
    void addStruct(std::unique_ptr<AST::Stmt> def);

    
    // if there are type errors, returns a vec of the error messages
    std::optional<std::vector<std::string>> CheckTypes();
    std::optional<std::vector<std::string>> CheckFn(
        const AST::FnStmt* fn
        );
    std::optional<std::vector<std::string>> CheckProto(
        const AST::ProtoStmt* fn
        );
    
    // statements
    ty::Type VisitExprStmt(const AST::ExprStmt *stmt); // TODO
    ty::Type VisitProtoStmt(const AST::ProtoStmt *stmt); // TODO
    ty::Type VisitFnStmt(const AST::FnStmt *stmt); // TODO
    ty::Type VisitVarDeclStmt(const AST::VarDeclStmt *stmt); // TODO
    ty::Type VisitStructDefStmt(const AST::StructDefStmt *stmt); // TODO
    //ty::Type VisitStructFieldStmt(const AST::StructFieldStmt *stmt); // TODO
    ty::Type VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt); // TODO
    ty::Type VisitIfStmt(const AST::IfStmt *stmt); // TODO
    ty::Type VisitWhileStmt(const AST::WhileStmt *stmt); // TODO
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
    ty::Type VisitStructFieldExpr(const AST::StructFieldExpr *expr);
    ty::Type VisitErrExpr(const AST::ErrExpr *expr);
    ty::Type VisitAssignExpr(const AST::AssignExpr *expr);
    ty::Type VisitLogicalExpr(const AST::LogicalExpr *expr);
    ty::Type VisitUnaryExpr(const AST::UnaryExpr *expr);

    ty::Type GenError(const char *Str) {
      std::cerr << "Type Error: " << Str << std::endl;
      return ty::Type(ty::tyType::Ty_Err, true);
    }
    // map of custom types to field names and types
    std::unordered_map<std::string, std::vector<std::unique_ptr<AST::Stmt>>> m_Environment;

  private:
    std::vector<std::string> m_CustomTypes;

    std::vector<std::string> m_TypeErrors{};
    AST::Unit* m_Unit;

    std::string m_CurFunc;
    AST::ProtoStmt* getCurProto();

    void tyError(const std::string& msg);
  };
}
}
#endif
