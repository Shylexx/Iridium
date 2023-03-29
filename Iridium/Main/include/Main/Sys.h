#ifndef __IRIDIUM_SYS_H__
#define __IRIDIUM_SYS_H__

#include "llvm/Target/TargetMachine.h"
namespace iridium {


class Target {
public:
  void InitObjTargets();
  bool SetCurrentTargetMachine();

  llvm::TargetMachine* targetMachine() const { return m_TargetMachine; }
  const std::string& targetTriple() const { return m_TargetTriple; }
private:
  llvm::TargetMachine *m_TargetMachine;
  std::string m_TargetTriple;
};

}

#endif
