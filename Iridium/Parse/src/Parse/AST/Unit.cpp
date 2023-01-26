#include "Parse/AST/Unit.h"
#include "Parse/AST/NodeType.h"
#include "Parse/AST/Stmt.h"
#include <iostream>

namespace iridium {
namespace AST {
void Unit::add(std::unique_ptr<AST::Stmt> stmt) {
  m_Items.push_back(std::move(stmt));
}

bool Unit::addProto(std::unique_ptr<AST::ProtoStmt> proto) {
  if (m_Functions.find(proto->name) == m_Functions.end()) {
    m_Functions.insert({proto->name, std::move(proto)});
    // return true when added successfully
    return true;
  }
  return false;
}

void Unit::protoErrMessage() {
  AST::Err *message = static_cast<AST::Err *>(m_ProtoErrors.back().get());
  std::cerr << "Syntax Error on line [" << message->m_SourceLine
            << "]: " << message->m_Message << std::endl;
}

bool Unit::error() {
  // if backmost item is an error stmt, or an expr stmt holding an errexpr, its
  // an error!
  if (m_Items.back()->node() == NodeType::ErrorNode) {
    return true;
  } else if (m_Items.back()->node() == NodeType::ExprStmtNode) {
    return static_cast<AST::ExprStmt *>(m_Items.back().get())->node() ==
           NodeType::ErrorNode;
  }
  return false;
}

void Unit::errMessage() {
  if (m_Items.back()->node() == NodeType::ErrorNode) {
    AST::Err *stmt = static_cast<AST::Err *>(m_Items.back().get());
    std::cerr << "Syntax Error in statement on line [" << stmt->m_SourceLine
              << "]: " << stmt->m_Message << std::endl;
  } else if (m_Items.back()->node() == NodeType::ExprStmtNode) {
    AST::ErrExpr *expr = static_cast<AST::ErrExpr *>(
        static_cast<AST::ExprStmt *>(m_Items.back().get())->Expression.get());
    std::cerr << "Syntax Error in statement on line [" << expr->Line
              << "]: " << expr->message() << std::endl;
  }
}

void Unit::prettyPrint() {
  // lord this requires reflection and I dont wanna
  // this will be a great lesson to add in the final report
  std::cout << "Unit {" << std::endl;
  std::cout << "}" << std::endl;
}

} // namespace AST
} // namespace iridium
