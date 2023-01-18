#include "Codegen/Codegen.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

namespace iridium {
  Codegen::Codegen() {
    m_Context = std::make_unique<llvm::LLVMContext>();
    m_Builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_Context));
    m_Module = std::make_unique<llvm::Module>("Module", *m_Context);
  }

  void Codegen::GenUnit(AST::Unit& unit) {
    for(auto& decl : unit.m_Items) {
      decl->Accept(this);
    }

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
