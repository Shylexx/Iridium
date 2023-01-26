#include "Parse/AST/NodeType.h"
#include <llvm/ADT/ilist_node.h>

namespace iridium {
namespace AST {
  std::string NodeToString(NodeType nodeType) {
    switch(nodeType) {
      case NodeType::FnProtoNode:
        return "STMT:FNPROTO";
      case NodeType::ExprStmtNode:
        return "STMT:EXPRSTMT";
      case NodeType::ErrorNode:
        return "NODE:PARSEERROR";
      case NodeType::None:
        return "NODE:NONE";
      case NodeType::Expr:
        return "EXPR:NONE";
      case NodeType::GlobVarDeclNode:
        return "STMT:GLOBVARDECL";
      case NodeType::VarDeclNode:
        return "STMT:VARDECL";
      default:
        return "NODE:UNIMPL";
    }
  }
}
}
