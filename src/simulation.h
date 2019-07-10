#pragma once

#include "generic.hpp"
#include "truthvalue.h"

namespace simulation {
	class Implementation;
	using Engine = GenericSimulator<TruthValue, Implementation>;

	class Implementation {
	  public:
		static void initialize(std::vector<TruthValue> &state);
		static void on_operator(ast::Operator, Engine::OperandStack &stack);
	};

	using StackMachine = Engine::StackMachine;
	using Circuit = Engine::Circuit;

	void run(const ast::Module &);
} // namespace simulation