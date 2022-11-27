add_library(IridiumLex STATIC
  src/Lex/Lexer.cpp
  src/Lex/TokType.cpp
  src/Lex/Token.cpp
)

target_include_directories(IridiumLex
  PUBLIC
  include
)


