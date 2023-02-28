#include "Codegen/Codegen.h"
#include "Parse/AST/Stmt.h"
#include "Parse/Type/Type.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Verifier.h>

#include <iostream>

namespace iridium {
    void Codegen::VisitProtoStmt(const AST::ProtoStmt *stmt) {
	std::cerr << "This should never get visited!" << std::endl;
    }

    void Codegen::VisitFnStmt(const AST::FnStmt* stmt) {
	// Function Prototype 
	AST::ProtoStmt* prototype = static_cast<AST::ProtoStmt*>(stmt->Proto.get());

	std::vector<llvm::Type*> types;
	for(int i = 0; i < prototype->params.size(); i++) {
	    std::cerr << ty::to_string(prototype->params[i].second) << std::endl;
	    types.push_back(from_Ty(prototype->params[i].second));
	}

	llvm::FunctionType* FType = llvm::FunctionType::get(from_Ty(prototype->retType), types, false);

	llvm::Function* func = llvm::Function::Create(FType, llvm::Function::ExternalLinkage, prototype->name, *m_Module);
	m_CurFunc = func;
	m_FuncMap[stmt] = m_CurFunc;

	// make names for args
	unsigned Idx = 0;
	for (auto& Arg: func->args())
	    std::cerr << prototype->params[Idx++].first.getString() << std::endl;
	    //Arg.setName(prototype->params[Idx++].first.getString());

	// Function Definition

	if(!func->empty()) {
	    std::cerr << "Function cannot be redefined!" << std::endl;
	}

	llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*m_Context, "entry", func);
	m_Builder->SetInsertPoint(basicBlock);

	// Making params available to the function
	

	m_NamedValues.clear();
	for(int i = 0; i < stmt->Proto->params.size(); i++) {
	    llvm::AllocaInst* alloca = m_Builder->CreateAlloca(from_Ty(stmt->Proto->params[i].second));
	    m_NamedValues[stmt->Proto->params[i].first.getString()] = alloca;
	    m_Builder->CreateStore(func->getArg(i), alloca);
	}

	stmt->body->Accept(this);

	// Return type
	// if no return statement, return Void
	// m_Builder->CreateRet();
	if(func->getReturnType()->isVoidTy()) {
	    m_Builder->CreateRetVoid();
	} 

	llvm::verifyFunction(*func);

	/*
	if(OPTIMIZE)
	    m_FPM->run(*func);
	*/
    }
}
