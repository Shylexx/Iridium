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

void Target::InitObjTargets() {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
}

bool Target::SetCurrentTargetMachine() {
  m_TargetTriple = llvm::sys::getDefaultTargetTriple();

  std::string err;
  auto target = llvm::TargetRegistry::lookupTarget(m_TargetTriple, err);
  if (!target) {
    llvm::errs() << err;
    return false;
  }


  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opts;
  auto RM = llvm::Optional<llvm::Reloc::Model>();
  m_TargetMachine = target->createTargetMachine(m_TargetTriple, CPU, Features, opts, RM);
  return true;
}

}
