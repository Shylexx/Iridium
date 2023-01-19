add_executable(IridiumMain
  src/Main/IridiumDriver.cpp
  src/Main/IridiumEntry.cpp
)

target_include_directories(IridiumMain
  PUBLIC
  include
)

target_link_libraries(IridiumMain
  PUBLIC
  IridiumCodegen
)
