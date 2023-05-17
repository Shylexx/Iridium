#include "Codegen/Codegen.h"
#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/PassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Scalar/LoopPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <optional>

namespace iridium {
  Codegen::Codegen() {
  }

  void Codegen::GenUnit(AST::Unit& unit) {
    std::cerr << "Generate Structs" << std::endl;
    CreateSymbols(unit);
    PrintIR();
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
    
    /*
    m_FPM->add(llvm::createPromoteMemoryToRegisterPass());
    m_FPM->add(llvm::createInstructionCombiningPass());
    m_FPM->add(llvm::createReassociatePass());
    m_FPM->add(llvm::createGVNPass());
    m_FPM->add(llvm::createCFGSimplificationPass());

    m_FPM->doInitialization();
    */
  }

  void Codegen::Optimize() {
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;

    llvm::PipelineTuningOptions options;
    options.LoopInterleaving = true;
    options.LoopUnrolling = true;
    options.LoopVectorization = true;
    options.SLPVectorization = true;
    options.MergeFunctions = true;
    llvm::PassBuilder passBuilder;

    //fam.registerPass([&] { return passBuilder.buildDefaultAAPipeline(); });
    passBuilder.registerModuleAnalyses(mam);
    passBuilder.registerCGSCCAnalyses(cgam);
    passBuilder.registerFunctionAnalyses(fam);
    passBuilder.registerLoopAnalyses(lam);
    passBuilder.crossRegisterProxies(lam, fam, cgam, mam);
    llvm::ModulePassManager mpm;
    mpm = passBuilder.buildO0DefaultPipeline(llvm::OptimizationLevel::O0);

    mpm.run(*m_Module, mam);
  }

  void Codegen::PrintIR() {
    llvm::errs() << *m_Module;
    fprintf(stderr, "\n");
  }

  llvm::Type* Codegen::from_Ty(ty::Type type) {
    std::cerr << "Getting llvm Type" << std::endl;
    switch(type.type()) {
      case ty::tyType::Ty_Bool:
	return llvm::Type::getInt1Ty(*m_Context);
      case ty::tyType::Ty_i64:
	return llvm::Type::getInt64Ty(*m_Context);
      case ty::tyType::Ty_i32:
	std::cerr << "using i32" << std::endl;
	return llvm::Type::getInt32Ty(*m_Context);
      case ty::tyType::Ty_f64:
	return llvm::Type::getDoubleTy(*m_Context);
      case ty::tyType::Ty_f32:
	return llvm::Type::getFloatTy(*m_Context);
      case ty::tyType::Ty_Void:
	return llvm::Type::getVoidTy(*m_Context);
      case ty::tyType::Ty_Struct:
	return m_Structs[type.name()];
      default:
	return nullptr;
    }
  }
}
