#include "Parse/Type/Type.h"
#include "Lex/TokType.h"
#include "llvm/IR/Type.h"

namespace iridium {
namespace ty {
  Type from_tok(tok::TokType tok) {
    switch(tok) {
      case(tok::TokType::f64):
        return Type::Ty_f64;
      case(tok::TokType::f32):
        return Type::Ty_f32;
      case(tok::TokType::i64):
        return Type::Ty_i64;
      case(tok::TokType::i32):
        return Type::Ty_i32;
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

  std::string to_string(Type type) {
    switch(type) {
      case(Type::Ty_Bool):
        return "TY_Bool";
      case(Type::Ty_f64):
        return "TY_f64";
      case(Type::Ty_f32):
        return "TY_f32";
      case(Type::Ty_i64):
        return "TY_i64";
      case(Type::Ty_i32):
        return "TY_i32";
      case(Type::Ty_Void):
        return "TY_Void";
      case(Type::Ty_Err):
        return "TY_Err";
      default:
        return "READERR";
    }
  }
} //namespace ty
} //namespace iridium
