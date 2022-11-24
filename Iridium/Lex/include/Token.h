#ifndef __IRIDIUM_TOKEN_H__
#define __IRIDIUM_TOKEN_H__

#include "TokType.h"
#include <variant>
#include <string>
#include <optional>
#include <cstdint>

namespace iridium {
  namespace tok {
    class IToken {
    public:
      IToken(std::uint32_t offset, std::uint32_t size) : m_Offset(offset), m_Size(size) {}

      std::uint32_t getOffset() const { return m_Offset; }
      std::uint32_t getSize() const { return m_Size; }

    private:
      std::uint32_t m_Offset;
      std::uint32_t m_Size;
    };

    // General purpose Token class
    class Token : public IToken {
    public:
      Token(uint32_t offset, uint32_t size, TokType tokenType, std::string string)
        : IToken(offset, size), m_TokType(tokenType), m_ValueString(string) {}
      Token(uint32_t offset, uint32_t size, TokType tokenType, std::int64_t i64)
        : IToken(offset, size), m_TokType(tokenType), m_Valuei64(i64) {}
      Token(uint32_t offset, uint32_t size, TokType tokenType, double f64)
        : IToken(offset, size), m_TokType(tokenType), m_Valuef64(f64) {}

      TokType getTokType() const { return m_TokType; }
      std::string getString() { return m_ValueString.value(); }
      std::int64_t geti64() { return m_Valuei64.value(); }
      double getf64() { return m_Valuef64.value(); }
      bool hasString() { return m_ValueString.has_value(); }
      bool hasi64() { return m_Valuei64.has_value(); }
      bool hasf64() { return m_Valuef64.has_value(); }
    private:
      TokType m_TokType;
      // Holds the value of the token( if it is a number or string)
      std::optional<std::string> m_ValueString;
      std::optional<std::int64_t> m_Valuei64;
      std::optional<double> m_Valuef64;

    };

    // Token class for user defined identifiers 
    class TokenIdentifier : public IToken {
    public:
      TokenIdentifier(Token& token)
        : IToken(token.getOffset(), token.getSize()), m_Identifier(token.getString()) {}

      TokType getTokenType() const { return TokType::Identifier; }
      std::string_view getIdentifier() const { return m_Identifier; }
    private:
      std::string m_Identifier;
    };
  } // namespace tok
} // namespace iridium
#endif
