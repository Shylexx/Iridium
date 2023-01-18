#include "Codegen/Codegen.h"
#include "Lex/TokType.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Stmt.h"
#include "llvm/IR/Function.h"

#include <iostream>

namespace iridium {
llvm::Value *Codegen::VisitIntExpr(const AST::IntExpr *expr) {
  return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_Context)),
                                      expr->Val);
}

llvm::Value *Codegen::VisitBoolExpr(const AST::BoolExpr *expr) {
  return llvm::ConstantInt::getSigned((llvm::Type::getInt1Ty(*m_Context)),
                                      expr->Val);
}

llvm::Value *Codegen::VisitBinaryExpr(const AST::BinaryExpr *expr) {
  llvm::Value *left = expr->LHS->Accept(this);
  llvm::Value *right = expr->LHS->Accept(this);
  if (!left || !right) {
    return nullptr;
  }

  switch (expr->op) {
  case tok::TokType::Plus:
    return m_Builder->CreateAdd(left, right, "addtmp");
  case tok::TokType::Minus:
    return m_Builder->CreateSub(left, right, "subtmp");
  case tok::TokType::Asterisk:
    return m_Builder->CreateMul(left, right, "multmp");
  default:
    return nullptr; // RETURN ERROR TODO
  }
}

llvm::Value *Codegen::VisitCallExpr(const AST::CallExpr *expr) {
    llvm::Function* callee = m_Module->getFunction(expr->Callee);
    if (!callee) {
	std::cerr << "Can't Call nonexistent function!" << std::endl;
    }

    // arg mismatch err
    if (callee->arg_size() != expr->Args.size()) {
	std::cerr << "Function parameter count incorrect" << std::endl;
    }

    std::vector<llvm::Value*> ArgsV;
    for(unsigned i = 0, e = expr->Args.size(); i != e; ++i) {
	ArgsV.push_back(expr->Args[i]->Accept(this));
	if (!ArgsV.back()) {
	    return nullptr;
	}
    }

    return m_Builder->CreateCall(callee, ArgsV, "calltmp");
}

llvm::Value* Codegen::VisitReturnExpr(const AST::ReturnExpr *expr) {
  llvm::Value* returnVal = expr->Val->Accept(this);
  m_Builder->CreateRet(returnVal);
  m_Builder->ClearInsertionPoint();
  return returnVal;
}

} // namespace iridium
