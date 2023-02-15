#ifndef __IRIDIUM_DRIVER_H__
#define __IRIDIUM_DRIVER_H__

#include "Codegen/Codegen.h"
#include "Parse/Parser.h"
#include <string>


namespace iridium {
	class Driver {
	public:
		void CompileFile(const std::string& path);
		Driver();
	private:
		std::string ReadFileToString(const std::string& path);

		Parser m_Parser;
		Codegen m_Codegen;
	};
}

#endif
