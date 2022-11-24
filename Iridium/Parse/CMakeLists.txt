add_library(IridiumParse STATIC
  src/Parser.cpp
)

target_include_directories(IridiumParse
  PUBLIC
  include
)

target_link_libraries(IridiumParse
  PUBLIC
  IridiumLex
)
