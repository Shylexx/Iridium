#include "Main/IridiumDriver.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <system_error>

namespace iridium {
  void Driver::CompileFile(const std::string &path) {
    m_Codegen.InitModuleAndFPM();
    std::string source = ReadFileToString(path);
    std::cerr << "Parsing File of name: " << path << std::endl;
    std::cerr << source << std::endl;

    if(m_Parser.ParseFile(source)) {
      std::cerr << "Generating Code..." << std::endl;
      m_Codegen.GenUnit(m_Parser.m_CurUnit);
      m_Codegen.PrintIR();

      m_Target.InitObjTargets();
      WriteToObjectFile();

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

  bool Driver::WriteToObjectFile() {
    m_Target.SetCurrentTargetMachine();
    auto filename = "output.o";

    std::error_code EC;

    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    if(EC) {
      llvm::errs() << "Could not open output file: " << EC.message() << "\n";
      return false;
    }
    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (m_Target.targetMachine()->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
      llvm::errs() << "TargetMachine can't emit a file of this type";
      return 1;
    }

    pass.run(*m_Codegen.module());
    dest.flush();

    return true;
  }
}
