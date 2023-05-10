#ifndef __IRIDIUM_DRIVER_H__
#define __IRIDIUM_DRIVER_H__

#include "Codegen/Codegen.h"
#include "Main/Opts.h"
#include "Parse/Parser.h"
#include "Main/Sys.h"
#include <string>


namespace iridium {
	class Driver {
	public:
		void CompileFile(const std::string& path);
		bool WriteToObjectFile();
		Opts opts{};
	private:
		std::string ReadFileToString(const std::string& path);

		Parser m_Parser;
		Codegen m_Codegen;
		Target m_Target;
	};
}

#endif
