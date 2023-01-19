#include "Main/IridiumDriver.h"

int main(int argc, char*argv[]) {
  iridium::Driver driver;
  driver.CompileFile(argv[0]);
  return 0;
}

