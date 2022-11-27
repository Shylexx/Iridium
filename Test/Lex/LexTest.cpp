#include <gtest/gtest.h>

#include <Lex/Token.h>
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
	std::cerr << lexer.DumpTokenTypes() << std::endl;
	EXPECT_EQ(lexer.tokenCount(), 4);
	EXPECT_EQ(lexer.errorCount(), 0);
}

TEST(LexTest, LexPunctuators) {
	iridium::Lexer lexer;
	lexer.LexString("fn, (test), fn");
	std::cerr << lexer.DumpTokenTypes() << std::endl;
	std::cerr << "Tokens Lexed: " << lexer.tokenCount() << std::endl;
	EXPECT_EQ(lexer.tokenCount(), 8);
	EXPECT_EQ(lexer.errorCount(), 0);
}

TEST(LexTest, StringsNeedEnding) {
	iridium::Lexer lexer;
	lexer.LexString("\" fn test, fn");
	lexer.syntaxErrorsToCerr();
	ASSERT_EQ(lexer.errorCount(), 1);
}
