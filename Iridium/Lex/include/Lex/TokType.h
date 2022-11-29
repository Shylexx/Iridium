#ifndef __IRIDIUM_TOKTYPE_H__
#define __IRIDIUM_TOKTYPE_H__

#include <cstdint>

namespace iridium {
  namespace tok {
      // Basic Tokens
    enum class TokType : std::uint8_t {
      EndOfFile,
      SyntaxError,
      Identifier,

      // Comments
      LineComment,

      // Keywords
      If,
      Else,
      For,
      While,
      Fn,
      Extern,
      Return,
      // Reserved For later use
      Struct,
      Enum,
      Trait,
      Static,

      // Literal Keywords
      i64KW,
      f64KW,
      StringKW,
    
      // Literals
      i64,
      f64,
      String,
      
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
      Equal,
      NotEqual,
      LessThan,
      GreaterThan,
      LessOrEqual,
      GreaterOrEqual,

      // Binop Calculation
      Plus,
      Minus,
      Asterisk,
      Slash,

      // 
      Hash
    };

  } // namespace tok
} // namespace iridium

#endif
