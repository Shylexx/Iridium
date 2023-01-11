#include "Codegen/Codegen.h"
#include "llvm/IR/LLVMContext.h"

namespace iridium {
	Codegen::Codegen() {
		m_Context = std::make_unique<llvm::LLVMContext>();
		m_Builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_Context));
		m_Module = std::make_unique<llvm::Module>("Module", *m_Context);
	}

	llvm::Value* Codegen::codegen(const AST::IntExpr& expr) {
		return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_Context)), expr.Val);
	}
}
