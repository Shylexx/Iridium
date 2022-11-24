add_library(IridiumMain STATIC
  src/IridiumEntry.cpp
)

target_include_directories(IridiumMain
  PUBLIC
  include
)

target_link_libraries(IridiumMain
  PUBLIC
  IridiumLex
)
