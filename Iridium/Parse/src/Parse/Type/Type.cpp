#include "Parse/Type/Type.h"
#include "Lex/TokType.h"
#include "llvm/IR/Type.h"

namespace iridium {
namespace ty {
  Type from_tok(tok::TokType tok) {
    switch(tok) {
      case(tok::TokType::f64):
        return Type::Ty_f64;
      case(tok::TokType::i64):
        return Type::Ty_i64;
      case(tok::TokType::Bool):
        return Type::Ty_Bool;
      default:
        return Type::Ty_Err;
    }
  }

  Type from_keyword(tok::TokType tok) {
    switch(tok) {
      case(tok::TokType::f64KW):
        return Type::Ty_f64;
      case(tok::TokType::f32KW):
        return Type::Ty_f64;
      case(tok::TokType::i64KW):
        return Type::Ty_i64;
      case(tok::TokType::i32KW):
        return Type::Ty_i32;
      case(tok::TokType::BoolKW):
        return Type::Ty_Bool;
      default:
        return Type::Ty_Err;
    }
  }
} //namespace ty
} //namespace iridium
