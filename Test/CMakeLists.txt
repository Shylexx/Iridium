cmake_minimum_required(VERSION 3.23)
project(IridiumTest VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For windows: prevent overriding parent projects compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(GTEST_SANITY
  src/Gtest_Sanity.cpp
)

target_link_libraries(GTEST_SANITY
  GTest::gtest_main
)

# LEXER TESTING
add_executable(LexTEST
  Lex/LexTest.cpp
)

target_link_libraries(LexTEST
  GTest::gtest_main
  IridiumLex
)

# Parser testing
add_executable(ParseTEST
  Parse/ParseTest.cpp
)

target_link_libraries(ParseTEST
  GTest::gtest_main
  IridiumParse
)

# Codegen testing
add_executable(CodegenTEST
  Codegen/CodegenTest.cpp
)

target_link_libraries(CodegenTEST
  GTest::gtest_main
  IridiumCodegen
)

include(GoogleTest)
gtest_discover_tests(GTEST_SANITY)

gtest_discover_tests(LexTEST)
gtest_discover_tests(ParseTEST)
gtest_discover_tests(CodegenTEST)
