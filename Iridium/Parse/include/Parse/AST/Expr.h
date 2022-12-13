#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__

#include "Parse/AST/Stmt.h"
#include <string>
#include <vector>
#include <memory>

namespace iridium {
	namespace AST {

		struct Expr : public Stmt {
		public:
			~Expr() override {}
		};

		struct BinaryExpr : public Expr {
		public:
			~BinaryExpr() override {}
		};

		struct BlockExpr : public Expr {
		public:
			~BlockExpr() override {}
		private:
			std::vector<std::unique_ptr<Stmt>> m_Body;
		};
	}
}
#endif