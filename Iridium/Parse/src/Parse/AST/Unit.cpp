#include "Parse/AST/Unit.h"
#include <iostream>

namespace iridium {
	namespace AST {
		void Unit::add(std::unique_ptr<AST::Stmt> stmt) {
			m_Items.push_back(std::move(stmt));
		}

		bool Unit::error() {
			Err* ast = dynamic_cast<Err*>(m_Items.back().get());
			return !(ast == nullptr);
		}

		void Unit::errMessage() {
			AST::Err* ast = dynamic_cast<AST::Err*>(m_Items.back().get());

			std::cerr << "Syntax Error on line [" << ast->m_SourceLine << "]: " << ast->m_Message << std::endl;
		}

		void Unit::prettyPrint() {
			// lord this requires reflection and I dont wanna 
			// this will be a great lesson to add in the final report
			std::cout << "Unit {" << std::endl;
				
			std::cout << "}" << std::endl;
		}

	} // namespace AST
} // namespace iridium
