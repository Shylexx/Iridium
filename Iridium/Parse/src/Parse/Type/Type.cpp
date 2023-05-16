#include "Parse/Type/Type.h"
#include "Lex/TokType.h"
#include "llvm/IR/Type.h"

namespace iridium {
namespace ty {
  Type from_tok(tok::TokType tok) {
    switch(tok) {
      case(tok::TokType::Float):
        return Type(tyType::Ty_f64);
      case(tok::TokType::Integer):
        return Type(tyType::Ty_i32);
      case(tok::TokType::Bool):
        return Type(tyType::Ty_Bool);
      default:
        return Type(tyType::Ty_Err);
    }
  }

  Type from_keyword(tok::TokType tok) {
    switch(tok) {
      case(tok::TokType::f64KW):
        return Type(tyType::Ty_f64);
      case(tok::TokType::f32KW):
        return Type(tyType::Ty_f32);
      case(tok::TokType::i64KW):
        return Type(tyType::Ty_i64);
      case(tok::TokType::i32KW):
        return Type(tyType::Ty_i32);
      case(tok::TokType::BoolKW):
        return Type(tyType::Ty_Bool);
      default:
        return Type(tyType::Ty_Err);
    }
  }

  std::string to_string(Type type) {
    switch(type.type()) {
      case(tyType::Ty_Bool):
        return "TY_Bool";
      case(tyType::Ty_f64):
        return "TY_f64";
      case(tyType::Ty_f32):
        return "TY_f32";
      case(tyType::Ty_i64):
        return "TY_i64";
      case(tyType::Ty_i32):
        return "TY_i32";
      case(tyType::Ty_Void):
        return "TY_Void";
      case(tyType::Ty_Struct):
        return "TY_Struct:[" + type.name() + "]";
      case(tyType::Ty_Err):
        return "TY_Err";
      default:
        return "READERR";
    }
  }
} //namespace ty
} //namespace iridium
