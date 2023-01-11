#include "Codegen/Codegen.h"
#include "Lex/TokType.h"
#include "Parse/AST/Expr.h"

namespace iridium {
	llvm::Value* Codegen::codegen(const AST::IntExpr& expr) {
		return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_Context)), expr.Val);
	}

	llvm::Value* Codegen::codegen(const AST::BoolExpr& expr) {
		return llvm::ConstantInt::getSigned((llvm::Type::getInt1Ty(*m_Context)), expr.Val);
	}

	llvm::Value* Codegen::codegen(const AST::BinaryExpr& expr) {
		llvm::Value* left = codegen(expr.LHS);
		llvm::Value* right = codegen(expr.RHS);
		if (!left || !right) {
			return nullptr;
		}

		switch(expr.op) {
			case tok::TokType::Plus: 
				return m_Builder->CreateAdd(left, right, "addtmp");
			case tok::TokType::Minus:
				return m_Builder->CreateSub(left, right, "subtmp");
			default:
				return nullptr; // RETURN ERROR TODO
		}
	}
}
