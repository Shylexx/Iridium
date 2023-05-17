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
    void Codegen::CreateSymbols(const AST::Unit &unit) {
	for(auto& structpair : unit.m_Context.m_Environment) {
	    m_Structs[structpair.first] = llvm::StructType::create(*m_Context, llvm::StringRef(structpair.first));
	}
    }
    void Codegen::VisitStructDefStmt(const AST::StructDefStmt *stmt) {
	std::cerr << "Start generating struct body" << std::endl;
	std::vector<llvm::Type*> fieldTypes;
	for(auto& field : stmt->m_Fields) {
	    AST::VarDeclStmt* fieldType = static_cast<AST::VarDeclStmt*>(field.get());
	    std::cerr << "Struct field type is" << ty::to_string(fieldType->type) << "\n";
	    fieldTypes.push_back(from_Ty(fieldType->type));
	}

	m_Structs[stmt->m_Name]->setBody(fieldTypes);
	std::cerr << "End generating struct body" << std::endl;
    }
}
