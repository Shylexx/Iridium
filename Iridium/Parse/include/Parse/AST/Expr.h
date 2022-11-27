#ifndef __IRIDIUM_EXPR_AST_H__
#define __IRIDIUM_EXPR_AST_H__


namespace iridium {
	namespace AST {
		class ASTVisitor;

		class ExprAST {
		public:
			virtual ~ExprAST() {}
			virtual void Accept(ASTVisitor* visitor) const = 0;
		};

		class BinaryExprAST : public ExprAST {
		public:
			void Accept(ASTVisitor* visitor) const override;
		};
	}
}
#endif