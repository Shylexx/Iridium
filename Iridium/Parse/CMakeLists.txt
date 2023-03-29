
add_library(IridiumParse STATIC
  src/Parse/Parser.cpp
  src/Parse/AST/ASTVisitor.cpp
  src/Parse/AST/Unit.cpp
  src/Parse/AST/NodeType.cpp
  src/Parse/Type/Type.cpp
  src/Parse/Type/Context.cpp
)

target_include_directories(IridiumParse
  PUBLIC
  include
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(IridiumParse
  PUBLIC
  IridiumLex
  LLVMCore
  LLVMTransformUtils
  LLVMAnalysis
  LLVMTarget
  LLVMAsmPrinter
  LLVMPasses
  LLVMMC
  LLVMCodeGen
  LLVMAnalysis
  LLVMSupport
)
