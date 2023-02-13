#ifndef __IRIDIUM_AST_NODETYPE_H__
#define __IRIDIUM_AST_NODETYPE_H__
#include <string>

namespace iridium {
namespace AST {
enum class NodeType {
  None,
  ExprStmtNode,
  ErrorNode,
  FnProtoNode,
  FnDeclNode,
  VarDeclNode,
  GlobVarDeclNode,
  IfStmtNode,
  Expr,
};

std::string NodeToString(NodeType nodeType);
}
}

#endif