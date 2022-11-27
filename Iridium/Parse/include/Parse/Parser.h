#ifndef __IRIDIUM_PARSER_H__
#define __IRIDIUM_PARSER_H__

#include "Parse/AST/Stmt.h"
#include "Parse/AST/Expr.h"
#include <string>

namespace iridium {
	class Parser {
	public:
		void ParseFile(const std::string& fileSource);
	};
}
#endif