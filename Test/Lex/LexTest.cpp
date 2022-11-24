#include <gtest/gtest.h>

#include <Token.h>
#include "TokType.h"


TEST(LexTest, CopyToken) {
	iridium::tok::Token testToken(0, iridium::tok::TokType::Identifier, "variableOne");
	iridium::tok::Token copiedToken(testToken);
	EXPECT_EQ(copiedToken.getTokType(), iridium::tok::TokType::Identifier);
	EXPECT_EQ(copiedToken.hasf64(), false);
	EXPECT_EQ(copiedToken.hasi64(), false);
	EXPECT_EQ(copiedToken.getString(), "variableOne");
}
