#ifndef __IRIDIUM_UNIT_H__
#define __IRIDIUM_UNIT_H__

#include "Parse/AST/Expr.h"
#include "Parse/AST/Stmt.h"
#include <memory>
#include <unordered_map>
#include <vector>


// The Unit class represents a Compilation unit!

namespace iridium {
namespace AST {
  class Unit {
  public:
    std::unordered_map<std::string, AST::FnStmt*> m_Functions;
    std::unordered_map<std::string, AST::VarDeclStmt*> m_Vars;
    std::vector<std::unique_ptr<AST::Stmt>> m_Items;
    void addGlobal();
    void add(std::unique_ptr<AST::Stmt> stmt);
    void prettyPrint();
    bool error();
    void errMessage();
  };
}
}
#endif
