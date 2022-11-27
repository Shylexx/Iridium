add_library(IridiumParse STATIC
  src/Parse/Parser.cpp
  src/Parse/AST/ASTVisitor.cpp
)

target_include_directories(IridiumParse
  PUBLIC
  include
)

target_link_libraries(IridiumParse
  PUBLIC
  IridiumLex
)
