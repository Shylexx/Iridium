#ifndef __IRIDIUM_TOKEN_H__
#define __IRIDIUM_TOKEN_H__

#include "TokType.h"
#include <variant>
#include <string>
#include <optional>
#include <cstdint>

namespace iridium {
  namespace tok {
    // General purpose Token class
    class Token {
    public:
      Token()
        : m_Size(0), m_TokType(tok::TokType::Default) {}
      Token(uint32_t size, TokType tokenType)
        : m_Size(size), m_TokType(tokenType) {}
      Token(uint32_t size, TokType tokenType, std::string string)
        : m_Size(size), m_TokType(tokenType), m_ValueString(string) {}
      // Line Number for making a descriptive Error Token
      Token(uint32_t size, TokType tokenType, std::string string, std::int64_t lineNum)
        : m_Size(size), m_TokType(tokenType), m_ValueString(string), m_Valuei64(lineNum) {}
      Token(uint32_t size, TokType tokenType, std::int64_t i64)
        : m_Size(size), m_TokType(tokenType), m_Valuei64(i64) {}
      Token(uint32_t size, TokType tokenType, double f64)
        : m_Size(size), m_TokType(tokenType), m_Valuef64(f64) {}

      TokType getTokType() const { return m_TokType; }
      std::string getString() const { return m_ValueString.value(); }
      std::int64_t geti64() { return m_Valuei64.value(); }
      double getf64() { return m_Valuef64.value(); }
      bool hasString() { return m_ValueString.has_value(); }
      bool hasi64() { return m_Valuei64.has_value(); }
      bool hasf64() { return m_Valuef64.has_value(); }

      std::uint32_t getSize() const { return m_Size; }
    private:
      TokType m_TokType;
      int m_SourceLine = 0;
      std::uint32_t m_Size;
      // Holds the value of the token( if it is a number or string)
      std::optional<std::string> m_ValueString;
      std::optional<std::int64_t> m_Valuei64;
      std::optional<double> m_Valuef64;

    };

  } // namespace tok
} // namespace iridium
#endif
