#include "Main/IridiumDriver.h"
#include <iostream>
#include <cxxopts.hpp>

int main(int argc, char*argv[]) {
  cxxopts::Options cli_opts("iridiumc", "Iridium Compiler");

  // temporarily using hardcoded defaut path
  cli_opts.add_options()
    ("h,help", "Print Usage Help")
    ("print-ir", "Print LLVM IR", cxxopts::value<bool>()->default_value("false"))
    ("f, file", "Entry point file to compile", cxxopts::value<std::string>())
    ("slib-path", "Standard library path, if not default (XDG_DATA)", cxxopts::value<std::string>()->default_value("/home/alex/.iridium"))
    ;

  auto opts = cli_opts.parse(argc,argv);

  if(opts.count("help")) {
    std::cout << cli_opts.help() << std::endl;
    return 0;
  }

  iridium::Driver driver;
  std::cerr << "Num of Args: " << argc << std::endl;
  if(argc > 1) {
    driver.CompileFile(argv[1]);
  } else {
    std::cerr << "No File path given" << std::endl;
  }
  return 0;
}

