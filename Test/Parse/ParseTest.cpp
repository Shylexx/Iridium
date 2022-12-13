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
