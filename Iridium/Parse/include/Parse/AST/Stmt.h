#ifndef __IRIDIUM_STMT_AST_H__
#define __IRIDIUM_STMT_AST_H__

#include <string>

namespace iridium {
	namespace AST {
		struct Stmt {
		public:
			virtual ~Stmt() {}
		};

		struct Err : public Stmt {
		public:
			~Err() override {}
			Err(const std::string& message) : m_Message(message) {}
			Err(const std::string& message, int line) : m_Message(message), m_SourceLine(line) {}
			const std::string& message() { return m_Message; }
			int sourceLine() { return m_SourceLine; }
		private:
			std::string m_Message;
			int m_SourceLine = 0;
		};

    struct FnStmt : public Stmt {
		public:
			~FnStmt() override {}
    };

		struct VarDeclStmt : public Stmt {
		public:
			~VarDeclStmt() override {}
			VarDeclStmt(const std::string& Name) : m_Name(Name) {}
		private:
			std::string m_Name;
		};

    struct IfStmt : public Stmt {
		public:
			~IfStmt() override {}
    };
	}
}
#endif
