
message("${LLVM_TARGETS_TO_BUILD}")

message("targets in main: ${llvm_targets}")

add_executable(IridiumMain
  src/Main/Opts.cpp
  src/Main/Sys.cpp
  src/Main/IridiumDriver.cpp
  src/Main/IridiumEntry.cpp
)

target_include_directories(IridiumMain
  PUBLIC
  include
  lib
)

target_link_libraries(IridiumMain
  PUBLIC
  IridiumCodegen
  ${llvm_targets}
)
