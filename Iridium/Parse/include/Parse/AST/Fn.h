#ifndef __IRIDIUM_FN_AST_H__
#define __IRIDIUM_FN_AST_H__

#include <string>
#include "Lex/Token.h"
#include "Parse/Type/Type.h"
#include "Parse/AST/ASTVisitor.h"
#include "Parse/AST/Expr.h"

namespace iridium {
namespace AST {
class FnProto  {
public:
  ~FnProto() {}
  FnProto(const std::string& name,
      std::vector<std::pair<tok::Token, ty::Type>> params,
      ty::Type type = ty::Type::Ty_Void)
    : name(name), params(std::move(params)), retType(type) {}

  FnProto(const std::string& name,
      std::vector<std::pair<tok::Token, ty::Type>> params,
      int line,
      ty::Type type = ty::Type::Ty_Void)
    : name(name), params(std::move(params)), retType(type) {}

  void Accept(ASTVisitor* visitor) const {
	  visitor->VisitFnProto(this);
  }

  int line() const { return m_SourceLine; }
  bool error = false;
  std::string errMsg;

  std::string name;
  std::vector<std::pair<tok::Token, ty::Type>> params;
  ty::Type retType = ty::Type::Ty_Void;
private:
  int m_SourceLine;
  NodeType nodeType = NodeType::FnProtoNode;
};

class FnDef {
public:
  ~FnDef() {}
  FnDef(std::unique_ptr<FnProto> prototype,
         std::unique_ptr<Expr> body,
         int line)
      : Proto(std::move(prototype)), body(std::move(body)), m_SourceLine(line) {}

  void Accept(ASTVisitor* visitor) const {
	  visitor->VisitFnDef(this);
  }

  std::unique_ptr<FnProto> Proto;
  std::unique_ptr<Expr> body;
  int line() const { return m_SourceLine; }
  bool error = false;
  std::string errMsg;
private:
  NodeType nodeType = NodeType::FnDeclNode;
  int m_SourceLine;
};
} // namespace AST
} // namespace iridium

#endif
