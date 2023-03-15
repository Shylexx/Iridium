#include "Main/Sys.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include "llvm/Target/TargetOptions.h"

#include <optional>
#include <string>

namespace iridium {

void InitObjTargets() {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
}

bool SetCurrentTargetMachine() {
  auto targetTriple = llvm::sys::getDefaultTargetTriple();

  std::string err;
  auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    llvm::errs() << err;
    return false;
  }

  return true;

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opts;
  auto RM = std::optional<llvm::Reloc::Model>();
  auto TargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opts, RM);
}

}
