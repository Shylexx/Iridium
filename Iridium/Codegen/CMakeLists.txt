find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_library(IridiumCodegen STATIC
	src/Codegen/Codegen.cpp
)

target_include_directories(IridiumCodegen
  PUBLIC
  include
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(IridiumCodegen
  PUBLIC
  IridiumParse
  LLVMCore
)
