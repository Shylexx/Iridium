#include <exception>
#include <gtest/gtest.h>

#include <Lex/Token.h>
#include <stdexcept>
#include "Lex/Lexer.h"


TEST(LexTest, CopyToken) {
	iridium::tok::Token testToken(0, iridium::tok::TokType::Identifier, "variableOne");
	iridium::tok::Token copiedToken(testToken);
	EXPECT_EQ(copiedToken.getTokType(), iridium::tok::TokType::Identifier);
	EXPECT_EQ(copiedToken.hasf64(), false);
	EXPECT_EQ(copiedToken.hasi64(), false);
	EXPECT_EQ(copiedToken.getString(), "variableOne");
}

TEST(LexTest, LexIdentifiersAndFunctions) {
	iridium::Lexer lexer;
	lexer.LexString("fn test fn");
	EXPECT_EQ(lexer.tokenCount(), 4);
	EXPECT_EQ(lexer.errorCount(), 0);
}

TEST(LexTest, LexPunctuators) {
	iridium::Lexer lexer;
	lexer.LexString("fn, (test), fn");
	EXPECT_EQ(lexer.tokenCount(), 8);
	EXPECT_EQ(lexer.errorCount(), 0);
}

TEST(LexTest, StringsNeedEnding) {
	iridium::Lexer lexer;
	lexer.LexString("\" fn test, fn");
	ASSERT_EQ(lexer.errorCount(), 1);
}

TEST(LexTest, KeywordLexing) {
	iridium::Lexer lexer;
	lexer.LexString("return extern, fn(fn)(, if while/ else *for + \n enum&String i64");
	EXPECT_EQ(lexer.curLine(), 2);
	EXPECT_EQ(lexer.errorCount(), 0);
	EXPECT_EQ(lexer.tokenCount(), 21);
}

TEST(LexTest, UnrecognisedChar) {
  iridium::Lexer lexer;
  lexer.LexString("testing ? char lexing");
  EXPECT_EQ(lexer.errorCount(), 1);
  EXPECT_EQ(lexer.tokenCount(), 5);
}

TEST(LexTest, NumberLexing) {
  iridium::Lexer lexer;
  lexer.LexString("304 20.01 02.3 3 01");
  EXPECT_EQ(lexer.errorCount(), 0);
  EXPECT_EQ(lexer.tokenCount(), 6);
}

TEST(LexTest, StringLexing) {
  iridium::Lexer lexer;
  lexer.LexString("\"Hi, I am string 1\" \"Hello, I am string 2\"");
  EXPECT_EQ(lexer.errorCount(), 0);
  EXPECT_EQ(lexer.tokenCount(), 3);
}
