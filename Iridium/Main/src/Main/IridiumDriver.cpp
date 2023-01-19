#include "Main/IridiumDriver.h"
#include <fstream>
#include <sstream>

namespace iridium {
	void Driver::CompileFile(const std::string &path) {
		std::string source = ReadFileToString(path);

		m_Parser.ParseFile(source);
		m_Codegen.GenUnit(m_Parser.m_CurUnit);
		m_Codegen.PrintIR();
	}

	std::string Driver::ReadFileToString(const std::string& path) {
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}
}
