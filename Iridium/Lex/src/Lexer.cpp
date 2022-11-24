#include "Lexer.h"
#include "TokType.h"
#include "Token.h"
#include <cctype>

namespace iridium {
  Lexer::Lexer(const std::string& sourceCode)
  : m_SourceCode(sourceCode)
  {
  }

  tok::Token Lexer::nextToken() {
    skipWhitespace();

    // Set the current character as the start of the token
    m_TokenStartIndex = m_SourceIndex;

    if (atEOF()) {
      addToken(tok::Token(0, tok::TokType::EndOfFile));
    }

    char nextChar = advance();

    if(std::isalpha(nextChar)) {
        return lexIdentifier();
    }

    if(std::isdigit(nextChar)) {
      return lexNumber();
    }

    switch (nextChar) {
      // single char punctuation
      case '(': return addToken(tok::Token(1, tok::TokType::OpenParen));
    }

    return tok::Token(0, tok::TokType::SyntaxError);
  }

  // Consume the character and move onto the next one
  char Lexer::advance() {
      m_SourceIndex++;
      return m_SourceCode[m_SourceIndex];
  }

  // Get the next character without consuming it
  char Lexer::peek() {
    return m_SourceCode[m_SourceIndex];
  }

  // Get the character ahead of the character we are currently looking at
  char Lexer::peekNext() {
    if(atEOF()) {
      return '\0';
    } else {
      return m_SourceCode[m_SourceIndex + 1];
    }
  }

  void Lexer::skipWhitespace() {
    for(;;) {
      switch(peek()) {
        case ' ':
        case '\t':
        case '\r':
          {
            advance();
            break;
          }
        case '\n':
          {
            m_CurrentLine++;
            advance();
            break;
          }
          // HANDLE LINE COMMENTS BY IGNORING THEM
        case '/':
          {
            if(peekNext() == '/') {
              while(peek() != '\n' && !atEOF()) {
                advance();
              }
            } else {
              return;
            }
          }
        default:
          {
            return;
          }
      }
    }
  } // skipWhiteSpace

  tok::Token Lexer::lexNumber() {
    while(std::isdigit(peek())) {
      advance();
    }

    // Find floating point numbers
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while(std::isdigit(peek())) {
          advance();
        }
        return addToken(tok::Token(currentTokenLength(), tok::TokType::f64));
    }

    // If no decimal point found, return integer
    return addToken(tok::Token(currentTokenLength(), tok::TokType::i64));
  }

  tok::Token Lexer::lexIdentifier() {
    // TODO
    switch (peek()) {
      case 'a':
        {
        }
    }
    return addToken(tok::Token(currentTokenLength(), tok::TokType::i64));
  }

  tok::Token Lexer::lexString() {
    // Save the string's start line in case of error
    int stringStartLine = m_CurrentLine;

    while(peek() != '"') {
      if(peek() == '\n') {
        m_CurrentLine++;
      } 
      advance();
    }

    if(atEOF()) {
      return addToken(tok::Token(0, tok::TokType::SyntaxError, "String Not Terminated", stringStartLine));
    }

    // Consume the closing '"'
    advance();
    return addToken(tok::Token(currentTokenLength(), tok::TokType::String, m_SourceCode.substr(m_TokenStartIndex + 1, currentTokenLength() - 1)));
  }

  int Lexer::currentTokenLength() {
    return m_SourceIndex - m_TokenStartIndex;
  }
  
  tok::Token Lexer::addToken(const tok::Token& tok) {
    m_Tokens.push_back(tok);
    return m_Tokens.back();
  }


} // Namespace iridium
