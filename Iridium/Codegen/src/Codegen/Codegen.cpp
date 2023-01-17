#include "Codegen/Codegen.h"
#include "llvm/IR/LLVMContext.h"

namespace iridium {
	Codegen::Codegen() {
		m_Context = std::make_unique<llvm::LLVMContext>();
		m_Builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_Context));
		m_Module = std::make_unique<llvm::Module>("Module", *m_Context);
	}
}
