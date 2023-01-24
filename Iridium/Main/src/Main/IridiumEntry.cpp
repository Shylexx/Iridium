#include "Main/IridiumDriver.h"
#include <iostream>

int main(int argc, char*argv[]) {
  iridium::Driver driver;
  std::cerr << "Num of Args: " << argc << std::endl;
  if(argc > 1) {
    driver.CompileFile(argv[1]);
  } else {
    std::cerr << "No File path given" << std::endl;
  }
  return 0;
}

