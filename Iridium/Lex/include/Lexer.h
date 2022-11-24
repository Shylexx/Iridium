#ifndef __IRIDIUM_LEXER_H__
#define __IRIDIUM_LEXER_H__

#include "Token.h"
#include <vector>

namespace iridium {
  class Lexer {
  public:
    Lexer(const std::string& sourceCode);

    // Advance the lexer through the token stream
    tok::Token nextToken();
  private:
    // All of the tokens lexed so far, in order
    std::vector<tok::Token> m_Tokens;
    // Current line of the source code
    int m_CurrentLine = 0;
    // Which token in the source is our cursor just before
    int m_CurrentTokenPosition = 0;
    // The last token lexed
    tok::Token m_LastToken {0, tok::TokType::EndOfFile};
    // The current source code stream
    std::string m_SourceCode;
    // The current source code character the character is before (cursor)
    int m_SourceIndex = 0;
    // The index of the start of the token currently being processed
    int m_TokenStartIndex = 0;

    tok::Token addToken(const tok::Token& token);

    tok::Token lexNumber();
    tok::Token lexIdentifier();
    tok::Token lexString();

    // Returns the amount of lexemes the current token is long
    int currentTokenLength();
    void skipWhitespace();
    // Return the character the cursor is currently just before before|char <-- char is returned 
    char peek();
    // Return the character after the peek character
    char peekNext();
    // Move the index cursor ahead
    char advance();
    bool atEOF();

  };
}

#endif
