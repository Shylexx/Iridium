add_library(IridiumLex STATIC
  src/Lexer.cpp
  src/TokType.cpp
  src/Token.cpp
)

target_include_directories(IridiumLex
  PUBLIC
  include
)


