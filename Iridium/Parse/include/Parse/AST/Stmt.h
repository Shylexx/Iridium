#ifndef __IRIDIUM_STMT_AST_H__
#define __IRIDIUM_STMT_AST_H__

#include "Lex/Token.h"
#include <string>
#include <vector>
#include <memory>

namespace iridium {
	namespace AST {

    enum NodeType {
      None,
      ExprStmt,
      Error,
      FnDecl,
      VarDecl,
      IfStmt,
      BinExpr,

    };

		class Stmt {
		public:
			virtual ~Stmt() {}
      NodeType nodeType = NodeType::None;
		};

		class Err : public Stmt {
		public:
			~Err() override {}
			Err(std::string message) : m_Message(message) {}
			Err(std::string message, int line) : m_Message(message), m_SourceLine(line) {}
      NodeType nodeType = NodeType::Error;
			std::string m_Message;
			int m_SourceLine = 0;
		};

    class FnStmt : public Stmt {
		public:
			~FnStmt() override {}
      FnStmt(const std::string& name, std::vector<std::pair<tok::Token, tok::Token>> params, std::vector<std::unique_ptr<Stmt>> body)
        : name(name), params(std::move(params)), body(std::move(body)) {}
      NodeType nodeType = NodeType::FnDecl;
      std::string name;
      std::vector<std::pair<tok::Token, tok::Token>> params;
      std::vector<std::unique_ptr<Stmt>> body;
    };

		class VarDeclStmt : public Stmt {
		public:
			~VarDeclStmt() override {}
			VarDeclStmt(const std::string& Name) : m_Name(Name) {}
      NodeType nodeType = NodeType::VarDecl;
			std::string m_Name;
		};

    class IfStmt : public Stmt {
		public:
      IfStmt() {}
			~IfStmt() override {}
      NodeType nodeType = NodeType::IfStmt;
    };

    class ExprStmt : public Stmt {
    public:
      ~ExprStmt() override {}
      NodeType nodeType = NodeType::ExprStmt;
    };
	}
}
#endif
