#include "Codegen/Codegen.h"
#include "llvm/IR/LLVMContext.h"
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include "llvm/Support/raw_ostream.h"
#include <iostream>

namespace iridium {
  Codegen::Codegen() {
  }

  void Codegen::GenUnit(AST::Unit& unit) {
    std::cerr << "Items in AST: " << unit.m_Items.size() << std::endl;
    for(auto& decl : unit.m_Items) {
      decl->Accept(this);
    }
  }

  void Codegen::InitModuleAndFPM() {
    m_Context = std::make_unique<llvm::LLVMContext>();
    m_Module = std::make_unique<llvm::Module>("Module", *m_Context);

    m_Builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_Context));

    m_FPM = std::make_unique<llvm::legacy::FunctionPassManager>(m_Module.get());
    
    if(OPTIMIZE) {
      m_FPM->add(llvm::createInstructionCombiningPass());
      m_FPM->add(llvm::createReassociatePass());
      m_FPM->add(llvm::createGVNPass());
      m_FPM->add(llvm::createCFGSimplificationPass());
    }
    
    m_FPM->doInitialization();
  }

  void Codegen::PrintIR() {
    llvm::errs() << *m_Module;
    fprintf(stderr, "\n");
  }

  llvm::Type* Codegen::from_Ty(ty::Type type) {
    switch(type) {
      case ty::Type::Ty_Bool:
	return llvm::Type::getInt1Ty(*m_Context);
      case ty::Type::Ty_i64:
	return llvm::Type::getInt64Ty(*m_Context);
      case ty::Type::Ty_i32:
	return llvm::Type::getInt32Ty(*m_Context);
      case ty::Type::Ty_f64:
	return llvm::Type::getDoubleTy(*m_Context);
      case ty::Type::Ty_f32:
	return llvm::Type::getFloatTy(*m_Context);
      case ty::Type::Ty_Void:
	return llvm::Type::getVoidTy(*m_Context);
      default:
	return nullptr;
    }
  }
}
