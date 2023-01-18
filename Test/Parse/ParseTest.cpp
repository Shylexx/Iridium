#include <gtest/gtest.h>

#include "Parse/AST/Unit.h"
#include "Parse/Parser.h"


TEST(ParseTest, ParseVariableDeclarations) {
	iridium::Parser parser;
	parser.ParseFile("i64 test;");
	parser.printSyntaxErrs();
	parser.printLexedToks();
}

TEST(ParseTest, VarDeclNeedsSemicolon) {
	iridium::Parser parser;
	parser.ParseFile("i64 noSemicolon");
}

TEST(ParseTest, FnDecl) {
  iridium::Parser parser;
  parser.ParseFile("fn testFunc(test: i64) {}");
}

TEST(ParseTest, BinopExpr) {
  iridium::Parser parser;
  parser.ParseFile("fn testFunction(test: i64) { return 3 + 3; }");
}
