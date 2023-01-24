#include "Main/IridiumDriver.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace iridium {
	void Driver::CompileFile(const std::string &path) {
		std::string source = ReadFileToString(path);
		std::cerr << "Parsing File of name: " << path << std::endl;
		std::cerr << source << std::endl;
		
		if(m_Parser.ParseFile(source)) {
			std::cerr << "Generating Code..." << std::endl;
			m_Codegen.GenUnit(m_Parser.m_CurUnit);
			m_Codegen.PrintIR();
		} else {
			std::cerr << "Errors during parsing." << std::endl;
		}
	}

	std::string Driver::ReadFileToString(const std::string& path) {
		std::ifstream t(path);
		std::string content((std::istreambuf_iterator<char>(t)),
				(std::istreambuf_iterator<char>()));
		return content;
	}
}
