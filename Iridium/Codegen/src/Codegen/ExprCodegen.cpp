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

llvm::Value* Codegen::VisitIfExpr(const AST::IfExpr *expr) {
  if (expr->Cond->retType != ty::Type::Ty_Bool)
    return GenError("Condition passed to if statement does not resolve to true or false");

  llvm::Value* CondV = expr->Cond->Accept(this);
  if (!CondV)
    return GenError("Invalid Condition passed to If Statement!");

  llvm::Function* parent = m_Builder->GetInsertBlock()->getParent();
  llvm::BasicBlock* ThenBlock = llvm::BasicBlock::Create(*m_Context, "then", parent);
  llvm::BasicBlock* ElseBlock = llvm::BasicBlock::Create(*m_Context, "else");
  llvm::BasicBlock* MergeBlock = llvm::BasicBlock::Create(*m_Context, "ifcont");

  m_Builder->CreateCondBr(CondV, ThenBlock, ElseBlock);

  m_Builder->SetInsertPoint(ThenBlock);
  stmt->Then->Accept(this);

  m_Builder->CreateBr(MergeBlock);
  // codegen of 'then' can change curent block, update it to for the PHI
  ThenBlock = m_Builder->GetInsertBlock();

  // else block
  parent->getBasicBlockList().push_back(ElseBlock);
  m_Builder->SetInsertPoint(ElseBlock);

  stmt->Else->Accept(this);

  m_Builder->CreateBr(MergeBlock);
  ElseBlock = m_Builder->GetInsertBlock();

  // Emit merge block
  parent->getBasicBlockList().push_back(MergeBlock);
  m_Builder->SetInsertPoint(MergeBlock);
  llvm::PHINode* phiNode = 
    m_Builder->CreatePHI(llvm::Type::getDoubleTy(*m_Context), 2, "iftmp");
  phiNode->addIncoming(ThenV, ThenBlock);
  phiNode->addIncoming(ElseV, ElseBlock);
  return phiNode;
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
