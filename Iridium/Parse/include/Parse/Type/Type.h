#ifndef __IRIDIUM__TYPE_H__
#define __IRIDIUM__TYPE_H__

#include "Lex/TokType.h"
#include "llvm/IR/Type.h"

namespace iridium {
namespace ty {
enum class tyType {
  Ty_i8,
  Ty_i64,
  Ty_i32,
  Ty_f64,
  Ty_f32,
  Ty_Bool,
  Ty_Struct,
  Ty_Void,
  Ty_Err,
};

class Type {
public:
  Type(tyType Type, bool primitive = true, const std::string& name = "")
    : m_Name(name), m_isPrimitive(primitive), m_tyType(Type) {}

  Type() = default;

  const std::string& name() const { return m_Name; }
  bool isPrimitive() const { return m_isPrimitive; }
  tyType type() const { return m_tyType; }

private:
  std::string m_Name;
  bool m_isPrimitive;
  tyType m_tyType;
};

// Equality operator
inline bool operator==(const Type& lhs, const Type& rhs) {
  if(lhs.isPrimitive() && rhs.isPrimitive()) {
    return lhs.type() == rhs.type();
  } else {
    return lhs.name() == rhs.name();
  }
}

Type from_tok(tok::TokType tok);
Type from_keyword(tok::TokType tok);
std::string to_string(Type type);
} //namespace ty
} //namespace iridium

#endif
