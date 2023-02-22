#include "Codegen/Codegen.h"
#include <iostream>

namespace iridium {
  void Codegen::VisitExprStmt(const AST::ExprStmt *stmt) {
    //std::cerr << "Visiting Expr Stmt" << std::endl;
    stmt->Expression->Accept(this);
  }

  void Codegen::VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt) {
  }

  void Codegen::VisitVarDeclStmt(const AST::VarDeclStmt *stmt) {
  }

  void Codegen::VisitBlockStmt(const AST::BlockStmt *stmt) {
    for(auto& bodyStmt : stmt->body) {
      bodyStmt->Accept(this);
    }
  }

  void Codegen::VisitErrStmt(const AST::Err *stmt) {
    std::cerr << stmt->m_Message << std::endl;
  }
}
