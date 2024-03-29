#include "Codegen/Codegen.h"
#include "Parse/Type/Type.h"
#include <iostream>

namespace iridium {
  void Codegen::VisitExprStmt(const AST::ExprStmt *stmt) {
    //std::cerr << "Visiting Expr Stmt" << std::endl;
    stmt->Expression->Accept(this);
  }

  void Codegen::VisitGlobVarDeclStmt(const AST::GlobVarDeclStmt *stmt) {
  }

  void Codegen::VisitVarDeclStmt(const AST::VarDeclStmt *stmt) {
    //llvm::IRBuilder<> temp(&m_CurFunc->getEntryBlock());

    // register variable and emit initializer
    std::cerr << "Create Alloca for var" << std::endl;
    std::cerr << "Var type is "  << ty::to_string(stmt->type) << std::endl;
    llvm::AllocaInst* alloca = m_Builder->CreateAlloca(from_Ty(stmt->type));
    llvm::Value* initialValue;
    if(stmt->m_Initializer) {
        std::cerr << "Generating initializer" << std::endl;
      initialValue = stmt->m_Initializer->Accept(this);
      if(!initialValue) {
        std::cerr << "Error generating code for variable initializer" << std::endl;
      }
      m_Builder->CreateStore(initialValue, alloca);
    } 
    std::cerr << "Alloca created for var" << std::endl;
    m_NamedValues[stmt->m_Name] = alloca;
  }

  void Codegen::VisitBlockStmt(const AST::BlockStmt *stmt) {
    for(auto& bodyStmt : stmt->body) {
      bodyStmt->Accept(this);
    }
  }

  void Codegen::VisitErrStmt(const AST::Err *stmt) {
    std::cerr << stmt->m_Message << std::endl;
  }

void Codegen::VisitWhileStmt(const AST::WhileStmt *stmt) {
  if (stmt->Cond->retType.type() != ty::tyType::Ty_Bool) {
    std::cerr << "Condition passed to while statement does not resolve to true or false" << std::endl;
    std::cerr << "While Stmt condition is of type: " << ty::to_string(stmt->Cond->retType) << std::endl;
  }

  llvm::Value* CondV = stmt->Cond->Accept(this);
  if(!CondV) {
    std::cerr << "Invalid Condition passed to While Statement!" << std::endl;
  }

  llvm::Function* parentFunction = m_Builder->GetInsertBlock()->getParent();

  llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*m_Context, "loop");
  llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(*m_Context, "loopend");

  m_Builder->CreateCondBr(CondV, loopBB, loopEndBB);

  // loop block
  parentFunction->getBasicBlockList().push_back(loopBB);
  m_Builder->SetInsertPoint(loopBB);
  
  stmt->Body->Accept(this);

  CondV = stmt->Cond->Accept(this);
  if(!CondV) {
    std::cerr << "Null condition expr for while statement" << std::endl;
  }

  // recursive codegen calls could change current block
  loopBB = m_Builder->GetInsertBlock();
  m_Builder->CreateCondBr(CondV, loopBB, loopEndBB);

  // loop end expr
  parentFunction->getBasicBlockList().push_back(loopEndBB);
  m_Builder->SetInsertPoint(loopEndBB);

}

void Codegen::VisitIfStmt(const AST::IfStmt *stmt) {
  if (stmt->Cond->retType.type() != ty::tyType::Ty_Bool) {
    std::cerr << "Condition passed to if statement does not resolve to true or false" << std::endl;
    std::cerr << "If Stmt condition is of type: " << ty::to_string(stmt->Cond->retType) << std::endl;
  }

  llvm::Value* CondV = stmt->Cond->Accept(this);
  if (!CondV)
    std::cerr << "Invalid Condition passed to If Statement!" << std::endl;

  llvm::Function* parent = m_Builder->GetInsertBlock()->getParent();
  llvm::BasicBlock* ThenBlock = llvm::BasicBlock::Create(*m_Context, "then", parent);
  llvm::BasicBlock* ElseBlock = llvm::BasicBlock::Create(*m_Context, "else");
  llvm::BasicBlock* MergeBlock = llvm::BasicBlock::Create(*m_Context, "ifcont");

  m_Builder->CreateCondBr(CondV, ThenBlock, ElseBlock);

  m_Builder->SetInsertPoint(ThenBlock);
  stmt->Then->Accept(this);

  // do blocks terminate themselves?
  bool thenTerminates = ThenBlock->back().isTerminator();

  // codegen of 'then' can change curent block, update it so the new block is below
  ThenBlock = m_Builder->GetInsertBlock();

  if(ThenBlock->size() == 0 || !thenTerminates) {
    m_Builder->CreateBr(MergeBlock);
  }

  // else block
  parent->getBasicBlockList().push_back(ElseBlock);
  m_Builder->SetInsertPoint(ElseBlock);

  stmt->Else->Accept(this);

  ElseBlock = m_Builder->GetInsertBlock();

  bool elseTerminates = ElseBlock->back().isTerminator();
  if(ElseBlock->size() == 0 || !elseTerminates) {
    m_Builder->CreateBr(MergeBlock);
  }

  // Emit merge block if either block continues past this point
  if(!thenTerminates || !elseTerminates) {
    parent->getBasicBlockList().push_back(MergeBlock);
    m_Builder->SetInsertPoint(MergeBlock);
  }
}

} //namespace iridium
