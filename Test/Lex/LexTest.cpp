#include <gtest/gtest.h>

#include <Token.h>
#include "TokType.h"


TEST(LexTest, CreateIdentifierTokenFromToken) {
  iridium::tok::Token testToken(32, 32, iridium::tok::TokType::Identifier, "identifierTest");
  iridium::tok::TokenIdentifier testIdentifier(testToken);
  EXPECT_EQ(testIdentifier.getTokenType(), iridium::tok::TokType::Identifier);
  EXPECT_EQ(testIdentifier.getIdentifier(), "identifierTest");
}
