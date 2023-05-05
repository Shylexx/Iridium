#ifndef __IRIDIUM__TYPE_H__
#define __IRIDIUM__TYPE_H__

#include "Lex/TokType.h"
#include "llvm/IR/Type.h"

namespace iridium {
namespace ty {
enum class Type {
  Ty_i8,
  Ty_i64,
  Ty_i32,
  Ty_f64,
  Ty_f32,
  Ty_Bool,
  Ty_Void,
  Ty_Err,
};

Type from_tok(tok::TokType tok);
Type from_keyword(tok::TokType tok);
std::string to_string(Type type);
} //namespace ty
} //namespace iridium

#endif
