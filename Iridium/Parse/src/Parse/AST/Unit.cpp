#include "Parse/AST/Unit.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Fn.h"
#include "Parse/AST/NodeType.h"
#include <iostream>

namespace iridium {
namespace AST {
void Unit::add(std::unique_ptr<AST::FnDef> fn) {
  m_Items.push_back(std::move(fn));
}

bool Unit::addProto(std::unique_ptr<AST::FnProto> proto) {
  if (m_Functions.find(proto->name) == m_Functions.end()) {
    m_Functions.insert({proto->name, std::move(proto)});
    // return true when added successfully
    return true;
  }
  return false;
}

void Unit::protoErrMessage() {
  std::cerr << "Syntax Error on line [" << m_ProtoErrors.back()->line()
            << "]: " << m_ProtoErrors.back()->errMsg << std::endl;
}

bool Unit::error() {
  if (m_Items.back()->error) {
    return true;
  }
  return false;
}

void Unit::errMessage() {
  if (m_Items.back()->error) {
    std::cerr << "Syntax Error in function on line [" << m_Items.back()->line()
              << "]: " << m_Items.back()->errMsg << std::endl;
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
