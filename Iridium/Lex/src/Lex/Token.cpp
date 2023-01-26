#include "Lex/Token.h"
#include <string>

namespace iridium {
namespace tok {
  std::string TokToString(tok::Token token) {
      std::string type;
      switch (token.getTokType()) {
      case tok::TokType::EndOfFile:
        type = "TOK::EOF";
        break;
      case tok::TokType::SyntaxError:
        type = "TOK::SYNTAXERR: " + token.getString();
        break;
      case tok::TokType::Identifier:
        type = "TOK::IDENTIFIER: " + token.getString();
        break;
      // Keywords
      case tok::TokType::Fn:
        type = "TOK::FN";
        break;
      case tok::TokType::If:
        type = "TOK::IF";
        break;
      case tok::TokType::Else:
        type = "TOK::ELSE";
        break;
      case tok::TokType::While:
        type = "TOK::WHILE";
        break;
      case tok::TokType::For:
        type = "TOK::FOR";
        break;
      case tok::TokType::Return:
        type = "TOK::RETURN";
        break;
      case tok::TokType::Extern:
        type = "TOK::EXTERN";
        break;
      case tok::TokType::Global:
        type = "TOK::GLOBAL";
        break;
      case tok::TokType::Enum:
        type = "TOK::ENUM";
        break;
      case tok::TokType::Struct:
        type = "TOK::STRUCT";
        break;
      case tok::TokType::i64KW:
        type = "TOK::i64KW";
        break;
      case tok::TokType::f64KW:
        type = "TOK::f64KW";
        break;
      case tok::TokType::i32KW:
        type = "TOK::i32KW";
        break;
      case tok::TokType::f32KW:
        type = "TOK::f32KW";
        break;
      case tok::TokType::StringKW:
        type = "TOK::STRINGKW";
        break;
      // Literals
      case tok::TokType::i64:
        type = "TOK::i64: " + std::to_string(token.geti64());
        break;
      case tok::TokType::i32:
        type = "TOK::i32: " + std::to_string(token.geti64());
        break;
      case tok::TokType::f64:
        type = "TOK::f64: " + std::to_string(token.getf64());
        break;
      case tok::TokType::f32:
        type = "TOK::f32: " + std::to_string(token.getf64());
        break;
      case tok::TokType::String:
        type = "TOK::STRING: " + token.getString();
        break;
      // Punctuators
      case tok::TokType::Period:
        type = "TOK::PERIOD";
        break;
      case tok::TokType::Comma:
        type = "TOK::COMMA";
        break;
      case tok::TokType::OpenParen:
        type = "TOK::OPENPAREN";
        break;
      case tok::TokType::CloseParen:
        type = "TOK::CLOSEPAREN";
        break;
      case tok::TokType::OpenBrace:
        type = "TOK::OPENBRACE";
        break;
      case tok::TokType::CloseBrace:
        type = "TOK::CLOSEBRACE";
        break;
      case tok::TokType::OpenBracket:
        type = "TOK::OPENBRACKET";
        break;
      case tok::TokType::CloseBracket:
        type = "TOK::CLOSEBRACKET";
        break;
      case tok::TokType::Ampersand:
        type = "TOK::AMPERSAND";
        break;
      case tok::TokType::Colon:
        type = "TOK::COLON";
        break;
      case tok::TokType::Semicolon:
        type = "TOK::SEMICOLON";
        break;
      case tok::TokType::Asterisk:
        type = "TOK::ASTERISK";
        break;
      case tok::TokType::Slash:
        type = "TOK::SLASH";
        break;
      case tok::TokType::Plus:
        type = "TOK::PLUS";
        break;
      case tok::TokType::Minus:
        type = "TOK::MINUS";
        break;
      case tok::TokType::Assignment:
        type = "TOK::ASSIGN";
        break;
      case tok::TokType::Equality:
        type = "TOK::EQUALITY";
        break;
      case tok::TokType::NotEquality:
        type = "TOK::NOTEQUALITY";
        break;
      case tok::TokType::GreaterThan:
        type = "TOK::GREATERTHAN";
        break;
      case tok::TokType::GreaterOrEqual:
        type = "TOK::GREATEROREQUAL";
        break;
      case tok::TokType::LessThan:
        type = "TOK::LESSTHAN";
        break;
      case tok::TokType::LessOrEqual:
        type = "TOK::LESSOREQUAL";
        break;
      case tok::TokType::Exclaim:
        type = "TOK::EXCLAIM";
        break;
      case tok::TokType::Arrow:
        type = "TOK::ARROW";
        break;
      case tok::TokType::Hash:
        type = "TOK::HASH";
        break;
      default:
        type = "TOK::UNKNOWN";
        break;
      }

    return type;
  }
}
}
