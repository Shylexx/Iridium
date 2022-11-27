#ifndef __IRIDIUM_AST_VISITOR_H__
#define __IRIDIUM_AST_VISITOR_H__

#include "Parse/AST/Expr.h"

namespace iridium {
	namespace AST {
		class ASTVisitor {
		public:
			void codegenBinExpr(BinaryExprAST* binExpr);
		};
	}
}
#endif