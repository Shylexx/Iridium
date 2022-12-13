#ifndef __IRIDIUM_AST_VISITOR_H__
#define __IRIDIUM_AST_VISITOR_H__


namespace iridium {
	namespace AST {

		struct FnStmt;
		struct IfStmt;

		class ASTVisitor {
		public:
			void VisitFnStmt(const FnStmt* element) const;
			void VisitIfStmt(const IfStmt* element) const;
		};
	}
}
#endif