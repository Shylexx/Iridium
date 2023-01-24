#include "Codegen/Codegen.h"
#include <iostream>

namespace iridium {
  void Codegen::VisitExprStmt(const AST::ExprStmt *stmt) {
    //std::cerr << "Visiting Expr Stmt" << std::endl;
    stmt->Expression->Accept(this);
  }

  void Codegen::VisitVarDeclStmt(const AST::VarDeclStmt *stmt) {
  }

  void Codegen::VisitIfStmt(const AST::IfStmt *stmt) {
  }

  void Codegen::VisitErrStmt(const AST::Err *stmt) {
    std::cerr << stmt->m_Message << std::endl;
  }
}
