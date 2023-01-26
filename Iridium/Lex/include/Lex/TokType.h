#ifndef __IRIDIUM_TOKTYPE_H__
#define __IRIDIUM_TOKTYPE_H__

#include <cstdint>
#include <string>

namespace iridium {
  namespace tok {
      // Basic Tokens
    enum class TokType : std::uint8_t {
      Default,
      EndOfFile,
      SyntaxError,
      ParseError,
      Identifier,

      // Comments
      LineComment,

      // Keywords
      If,
      Else,
      For,
      While,
      Fn,
      Global,
      Extern,
      Return,
      // Reserved For later use
      Struct,
      Enum,
      Trait,
      Static,

      // Literal Keywords
      i64KW,
      i32KW,
      f64KW,
      f32KW,
      StringKW,
      BoolKW,
    
      // Literals
      Bool,
      i64,
      i32,
      f64,
      f32,
      String,

      // Logical Operators
      Logical_OR,
      Logical_AND,
      
      // Punctuators
      Colon,
      Semicolon,
      Comma,
      Period,
      Ampersand,

      // Delimiters
      OpenParen,
      CloseParen,
      OpenBrace,
      CloseBrace,
      OpenBracket,
      CloseBracket,
      Assignment,

      // Unary Boolean
      Exclaim,
      // Binary Bool Op
      Equality,
      NotEquality,
      LessThan,
      GreaterThan,
      LessOrEqual,
      GreaterOrEqual,
      QuestionMark,

      // Binop Calculation
      Plus,
      Minus,
      Asterisk,
      Slash,

      // 
      Hash,
      Arrow
    };

  } // namespace tok
} // namespace iridium

#endif
