#include <gtest/gtest.h>

#include "Parse/Parser.h"
#include "Codegen/Codegen.h"

TEST(CodegenTest, FunctionGen) {
  iridium::Parser parser;
  parser.ParseFile("fn testFunc(test: i64) { return 3 + 3; }");
  iridium::Codegen gen;
  gen.GenUnit(parser.m_CurUnit);

}


