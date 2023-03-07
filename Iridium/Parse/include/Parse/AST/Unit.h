#ifndef __IRIDIUM_UNIT_H__
#define __IRIDIUM_UNIT_H__

#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include "Parse/Type/Context.h"
#include <memory>
#include <unordered_map>
#include <vector>


// The Unit class represents a Compilation unit!

namespace iridium {
namespace AST {
  class Unit {
  public:
    std::unordered_map<std::string, std::unique_ptr<AST::ProtoStmt>> m_Functions;
    std::vector<std::unique_ptr<AST::Stmt>> m_Items;
    std::vector<std::unique_ptr<AST::Stmt>> m_ProtoErrors;
    std::unordered_map<std::string, ty::Type> m_Vars;
    void protoErrMessage();
    void add(std::unique_ptr<AST::Stmt> stmt);
    // return false if failed (err stmt) or true if added to map
    bool addProto(std::unique_ptr<AST::ProtoStmt> proto);
    void prettyPrint();
    bool error();
    void errMessage();
    ty::Context m_Context = ty::Context(this);

  };
}
}
#endif
