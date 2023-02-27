#include "Codegen/Codegen.h"
#include "Lex/TokType.h"
#include "Parse/AST/Expr.h"
#include "Parse/AST/Stmt.h"
#include "Parse/Type/Type.h"
#include "llvm/IR/Function.h"

#include <iostream>
#include <llvm/IR/Instructions.h>

namespace iridium {
llvm::Value *Codegen::VisitIntExpr(const AST::IntExpr *expr) {
  std::cerr << "Visited Int: " << expr->Val << std::endl;
  //std::cerr << "Int visited" << std::endl;
  return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_Context)),
                                      expr->Val);
}

llvm::Value *Codegen::VisitFloatExpr(const AST::FloatExpr *expr) {
  return llvm::ConstantInt::getSigned((llvm::Type::getFloatTy(*m_Context)),
                                      expr->Val);
}

llvm::Value *Codegen::VisitBoolExpr(const AST::BoolExpr *expr) {
  return llvm::ConstantInt::getSigned((llvm::Type::getInt1Ty(*m_Context)),
                                      expr->Val);
}

llvm::Value* Codegen::VisitUnaryExpr(const AST::UnaryExpr *expr) {
  return nullptr;
}

llvm::Value *Codegen::VisitBinaryExpr(const AST::BinaryExpr *expr) {
  std::cerr << "Codegen Binary Expr" << std::endl;
  llvm::Value *left = expr->LHS->Accept(this);
  llvm::Value *right = expr->RHS->Accept(this);
  if (!left || !right) {
    if (!left) {
      std::cerr << "error generating lhs" << std::endl;
    } else {
      std::cerr << "error generating rhs" << std::endl;
    }
    return nullptr;
  }

  switch (expr->op) {
  case tok::TokType::Plus:
    return m_Builder->CreateAdd(left, right, "addtmp");
  case tok::TokType::Minus:
    return m_Builder->CreateSub(left, right, "subtmp");
  case tok::TokType::Asterisk:
    return m_Builder->CreateMul(left, right, "multmp");
  case tok::TokType::Equality: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpEQ(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpUEQ(left, right, "cmptmp");
      }
    }
  case tok::TokType::NotEquality: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpNE(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpUNE(left, right, "cmptmp");
      }
    }
  case tok::TokType::LessThan: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpSLT(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpULT(left, right, "cmptmp");
      }
    }
  case tok::TokType::LessOrEqual: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpSLE(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpULE(left, right, "cmptmp");
      }
    }
  case tok::TokType::GreaterThan: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpSGT(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpUGT(left, right, "cmptmp");
      }
    }
  case tok::TokType::GreaterOrEqual: {
      if(expr->LHS->retType == ty::Type::Ty_i32 || expr->LHS->retType == ty::Type::Ty_i64) {
	return m_Builder->CreateICmpSGE(left, right, "cmptmp");
      } else if (expr->LHS->retType == ty::Type::Ty_f32 || expr->LHS->retType == ty::Type::Ty_f64) {
	return m_Builder->CreateFCmpUGE(left, right, "cmptmp");
      }
    }
  default:
    return GenError("Invalid Binary Operator"); // RETURN ERROR TODO
  }
}

llvm::Value* Codegen::VisitLogicalExpr(const AST::LogicalExpr *expr) {
  return nullptr;
}

llvm::Value* Codegen::VisitErrExpr(const AST::ErrExpr *expr) {
  std::string message = "Expression Parsing Error: " + expr->message();
  return GenError(message.c_str());
}

llvm::Value* Codegen::VisitVarExpr(const AST::VarExpr *expr) {
  llvm::Value* V = m_NamedValues[expr->Iden];
  if(!V)
    GenError("Unknown Variable Name");
  return V;
}

llvm::Value* Codegen::VisitAssignExpr(const AST::AssignExpr *expr) {
  return nullptr;
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
  return returnVal;
}

} // namespace iridium
