#ifndef __IRIDIUM_UNIT_H__
#define __IRIDIUM_UNIT_H__

#include "Parse/AST/Expr.h"
#include "Parse/AST/Stmt.h"
#include <memory>
#include <vector>


// The Unit class represents a Compilation unit!

namespace iridium {
	namespace AST {
		class Unit {
		public:
			void addGlobal();
			void add(std::unique_ptr<AST::Stmt> stmt);
			void prettyPrint();
			bool error();
			void errMessage();
		private:
			std::vector<std::unique_ptr<AST::Stmt>> m_Items;

		};
	}
}
#endif