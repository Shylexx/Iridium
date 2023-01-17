#ifndef __IRIDIUM_CODEGEN_H__
#define __IRIDIUM_CODEGEN_H__

#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "Parse/AST/Expr.h"

// Code Generator Driver

namespace iridium {
	class Codegen {
	public:
		Codegen();
	private:
		std::unique_ptr<llvm::LLVMContext> m_Context;
		std::unique_ptr<llvm::IRBuilder<>> m_Builder;
		std::unique_ptr<llvm::Module> m_Module;

		llvm::Value* codegen(const AST::Expr& expr);

		// statements

		// expressions
		// primitives
		llvm::Value* codegen(const AST::IntExpr& expr);
		llvm::Value* codegen(const AST::BoolExpr& expr);

		llvm::Value* codegen(const AST::BinaryExpr& expr);

	};
}

#endif
