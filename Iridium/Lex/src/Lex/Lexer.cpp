#include "Lex/Lexer.h"
#include "Lex/TokType.h"
#include "Lex/Token.h"
#include <iostream>
#include <cctype>

namespace iridium {
  Lexer::Lexer()
  {
  }

  void Lexer::LexString(const std::string& source) {
    m_SourceCode = source;
    m_SourceIndex = 0;

    // Lex the first token
    nextToken();
    if (m_Tokens.back().getTokType() == tok::TokType::EndOfFile) {
      std::cerr << "No Tokens in Source file" << std::endl;
      return;
    }

    // Get all tokens in the file.
    while (m_Tokens.back().getTokType() != tok::TokType::EndOfFile) {
      auto tok = nextToken();

      // Not working?
      /*if (tok.getTokType() == tok::TokType::SyntaxError) {
        std::cerr << "Syntax Error: " << tok.getString() << " on line [" << tok.geti64() << "]" << std::endl;
      }*/
    }
  }

  void Lexer::addSource(const std::string &source) {
    m_SourceCode += source;
  }

  tok::Token Lexer::nextToken() {
    skipWhitespace();

    // Set the current character as the start of the token
    m_TokenStartIndex = m_SourceIndex;

    if (atEOF()) {
      return addToken(tok::Token(0, tok::TokType::EndOfFile));
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
      case ')': return addToken(tok::Token(1, tok::TokType::CloseParen));
      case '{': return addToken(tok::Token(1, tok::TokType::OpenBrace));
      case '}': return addToken(tok::Token(1, tok::TokType::CloseBrace));
      case ';': return addToken(tok::Token(1, tok::TokType::Semicolon));
      case ':': return addToken(tok::Token(1, tok::TokType::Colon));
      case ',': return addToken(tok::Token(1, tok::TokType::Comma));
      case '.': return addToken(tok::Token(1, tok::TokType::Period));
      case '+': return addToken(tok::Token(1, tok::TokType::Plus));
      case '*': return addToken(tok::Token(1, tok::TokType::Asterisk));
      case '/': return addToken(tok::Token(1, tok::TokType::Slash));
      case '&': return addToken(tok::Token(1, tok::TokType::Ampersand));
      // Two char punctuators
      case '-':
        return match('>') ? addToken(tok::Token(1, tok::TokType::Arrow)) : addToken(tok::Token(1, tok::TokType::Minus));
      case '!':
        return match('=') ? addToken(tok::Token(1, tok::TokType::NotEquality)) : addToken(tok::Token(1, tok::TokType::Exclaim));
      case '=':
        return match('=') ? addToken(tok::Token(1, tok::TokType::Equality)) : addToken(tok::Token(1, tok::TokType::Assignment));
      case '<':
        return match('=') ? addToken(tok::Token(1, tok::TokType::LessOrEqual)) : addToken(tok::Token(1, tok::TokType::LessThan));
      case '>':
        return match('=') ? addToken(tok::Token(1, tok::TokType::GreaterOrEqual)) : addToken(tok::Token(1, tok::TokType::GreaterThan));
    }

    std::string errorMessage = std::string("Unrecognised character '") + nextChar + "'";
    return addToken(tok::Token(1, tok::TokType::SyntaxError, errorMessage, m_CurrentLine));
  }

  // Consume the character and move onto the next one
  char Lexer::advance() {
      m_SourceIndex++;
      // Return last char as we move then return
      return m_SourceCode[m_SourceIndex-1];
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

  bool Lexer::match(char expected) {
    if (atEOF()) {
      return false;
    }

    if (m_SourceCode[m_SourceIndex] != expected) {
      return false;
    }

    // Advance if we match the expected character
    m_SourceIndex++;
    return true;
  }

  bool Lexer::atEOF() {
    return m_SourceIndex == m_SourceCode.length();
  }

  void Lexer::skipWhitespace() {
    if (atEOF()) {
      return;
    }
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


  void Lexer::skipToEndOfAlnum() {
    while (std::isalnum(peek())) {
      advance(); 
    }
  }

  tok::Token Lexer::lexNumber() {
    std::string numString;
    while(std::isdigit(peek())) {
      advance();
    }

    // Find floating point numbers
    if (peek() == '.' && std::isdigit(peekNext())) {
      // Consume the '.'
      advance();
        while(std::isdigit(peek())) {
          advance();
        }
        numString = m_SourceCode.substr(m_TokenStartIndex, currentTokenLength());
        return addToken(tok::Token(currentTokenLength(), tok::TokType::Float, std::stod(numString)));
    }

    // If no decimal point found, return integer
    numString = m_SourceCode.substr(m_TokenStartIndex, currentTokenLength());
    return addToken(tok::Token(currentTokenLength(), tok::TokType::Integer, (int64_t)std::stol(numString)));
  }

  tok::Token Lexer::lexIdentifier() {
    // TODO
    /*switch (m_SourceCode[m_TokenStartIndex]) {
    case 'f':
    {
      switch (m_SourceCode[m_TokenStartIndex + 1]) {
      case 'n': {
        skipToEndOfAlnum();
        return addToken(tok::Token(currentTokenLength(), tok::TokType::Fn));
      }
      case 'o': {
        skipToEndOfAlnum();
        return addToken(tok::Token(currentTokenLength(), tok::TokType::For));
      }
      case '6': {
        skipToEndOfAlnum();
        return addToken(tok::Token(currentTokenLength(), tok::TokType::f64));
      }
      default: {
        break;
      }
      }
    }
    case 'i':
    {
      switch (m_SourceCode[m_TokenStartIndex + 1]) {
      case 'f':
      {
        skipToEndOfAlnum();
        return addToken(tok::Token(currentTokenLength(), tok::TokType::If));
      }
      case '6':
      {
        skipToEndOfAlnum();
        return addToken(tok::Token(currentTokenLength(), tok::TokType::i64));
      }
      default: break;
      }
    }
    case 'e':
    {
      skipToEndOfAlnum();
      return addToken(tok::Token(currentTokenLength(), tok::TokType::Else));
    }
    case 'r':
    {
      skipToEndOfAlnum();
      return addToken(tok::Token(currentTokenLength(), tok::TokType::Return));
    }
    case 'S':
    {
      skipToEndOfAlnum();
      return addToken(tok::Token(currentTokenLength(), tok::TokType::String));
    }
    case 's':
    {
      skipToEndOfAlnum();
      return addToken(tok::Token(currentTokenLength(), tok::TokType::Struct));
    }
    case 'w':
    {
      skipToEndOfAlnum();
      return addToken(tok::Token(currentTokenLength(), tok::TokType::While));
    }
    default: break;
    }*/

    // Get the whole identifier string
    skipToEndOfAlnum();

    // Get the current token as a string
    std::string identifierString = m_SourceCode.substr(m_TokenStartIndex, currentTokenLength());

    tok::TokType toktype = stringToTokType(identifierString);

    if (toktype == tok::TokType::Identifier) {
      return addToken(tok::Token(currentTokenLength(), tok::TokType::Identifier, identifierString));
    }
    else {
      return addToken(tok::Token(currentTokenLength(), toktype));
    }
  }

  /*
  tok::Token Lexer::lexString() {
    // Save the string's start line in case of error
    int stringStartLine = m_CurrentLine;

    while (peek() != '"') {
      if (peek() == '\n') {
        m_CurrentLine++;
      }

      advance();

      if (atEOF()) {
        return addToken(tok::Token(0, tok::TokType::SyntaxError, "String Not Terminated", stringStartLine));
      }
    }

    // Consume the closing '"'
    advance();
    return addToken(tok::Token(currentTokenLength(), tok::TokType::String, m_SourceCode.substr(m_TokenStartIndex + 1, currentTokenLength() - 1)));
  }
  */

  int Lexer::currentTokenLength() {
    return m_SourceIndex - m_TokenStartIndex;
  }
  
  tok::Token Lexer::addToken(const tok::Token& tok) {
    m_Tokens.push_back(tok);
    return m_Tokens.back();
  }

  tok::TokType Lexer::stringToTokType(const std::string& str) {
    switch (str.size()) {
    case 2: {
      if (str == "if") {
        return tok::TokType::If;
      }
      else if (str == "fn") {
        return tok::TokType::Fn;
      }
      else {
        return tok::TokType::Identifier;
      }
    }
    case 3: {
      if (str == "for") {
        return tok::TokType::For;
      }
      else if (str == "i64") {
        return tok::TokType::i64KW;
      }
      else if (str == "f64") {
        return tok::TokType::f64KW;
      }
      else if (str == "i32") {
        return tok::TokType::i32KW;
      }
      else if (str == "f32") {
        return tok::TokType::f32KW;
      }
      else {
        return tok::TokType::Identifier;
      }
    }
    case 4: {
      if (str == "else") {
        return tok::TokType::Else;
      }
      else if (str == "enum") {
        return tok::TokType::Enum;
      } 
      else if (str == "bool") {
        return tok::TokType::BoolKW;
      }
      else {
        return tok::TokType::Identifier;
      }
    }
    case 5: {
      if (str == "while") {
        return tok::TokType::While;
      }
      else if (str == "struct") {
        return tok::TokType::Struct;
      }
      else {
        return tok::TokType::Identifier;
      }
    }
    case 6: {
      if (str == "extern") {
        return tok::TokType::Extern;
      }
      if (str == "global") {
        return tok::TokType::Global;
      }
      else if (str == "return") {
        return tok::TokType::Return;
      }
      /*
      else if (str == "String") {
        return tok::TokType::StringKW;
      }
      */
      else {
        return tok::TokType::Identifier;
      }
    }
    default: {
      return tok::TokType::Identifier;
    }
    }
  }

  std::string Lexer::DumpTokenTypes() {
    std::string tokenTypes;
    for (auto& token : m_Tokens) {
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
        /*
      case tok::TokType::StringKW:
        type = "TOK::STRINGKW";
        break;
        */
      // Literals
      case tok::TokType::Integer:
        type = "TOK::INT: " + std::to_string(token.geti64());
        break;
      case tok::TokType::Float:
        type = "TOK::FLOAT: " + std::to_string(token.getf64());
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
      tokenTypes += type + ", ";
    }

    return tokenTypes;
  }

  int Lexer::errorCount() {
    int errorCount = 0;
    for (auto& token : m_Tokens) {
      if (token.getTokType() == tok::TokType::SyntaxError) {
        errorCount++;
      }
    }
    return errorCount;
  }

  void Lexer::syntaxErrorsToCerr() {
    for (auto& tok : m_Tokens) {
      if (tok.getTokType() == tok::TokType::SyntaxError) {
        std::cerr << "Syntax Error: " << tok.getString() << " on line [" << tok.geti64() << "]" << std::endl;
      }
    }
  }


} // Namespace iridium
