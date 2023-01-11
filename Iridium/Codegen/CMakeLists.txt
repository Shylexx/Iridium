add_library(IridiumCodegen STATIC
	src/Codegen/Codegen.cpp
	src/Codegen/ExprCodegen.cpp
	src/Codegen/FnCodegen.cpp
)

target_include_directories(IridiumCodegen
  PUBLIC
  include
)

target_link_libraries(IridiumCodegen
  PUBLIC
  IridiumParse
)
