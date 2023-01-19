#include "Codegen/Codegen.h"
#include "Parse/Type/Type.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"

#include <iostream>

namespace iridium {
    void Codegen::VisitFnStmt(const AST::FnStmt* stmt) {
	std::vector<llvm::Type*> types;
	for(int i = 0; i < stmt->params.size(); i++) {
	    types.push_back(from_Ty(stmt->params[i].second));
	}

	llvm::FunctionType* FType = llvm::FunctionType::get(from_Ty(stmt->retType), types, false);

	llvm::Function* func = llvm::Function::Create(FType, llvm::Function::ExternalLinkage, stmt->name, *m_Module);

	// make names for args
	unsigned Idx = 0;
	for (auto& Arg: func->args())
	    Arg.setName(stmt->params[Idx++].first.getString());

	if(!func->empty()) {
	    std::cerr << "Function cannot be redefined!" << std::endl;
	}

	llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*m_Context, "entry", func);
	m_Builder->SetInsertPoint(basicBlock);

	// Making params available to the function
	/*
	NamedValues.clear();
	for(auto& Arg : func->args())
	    NamedValues[Arg.getName()] = &Arg;
	*/

	for(auto& bodyStmt : stmt->body) {
	    bodyStmt->Accept(this);
	}

	// Return type
	/*
	if (stmt->retType != ty::Type::Ty_Void) {
	    m_Builder->CreateRet(
	}
	*/
    }
}