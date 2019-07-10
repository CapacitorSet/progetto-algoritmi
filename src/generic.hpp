#pragma once

#include "ast.h"

template <typename T, class Implementation>
class GenericSimulator {
  public:
	using OperandStack = std::stack<T>;

	class StackMachine {
		ast::Expression expression;
		OperandStack operandStack;

		Implementation &impl;

	  public:
		StackMachine(const ast::Expression &expr, Implementation &impl)
		    : expression(expr), impl(impl) {}
		T evaluate(const std::vector<T> &inputs, const std::vector<T> &state,
		           const std::vector<T> &outputs);
	};

	class Circuit {
		ast::Module module;
		std::vector<T> _state;
		std::vector<T> _outputs;

		Implementation &impl;

	  public:
		Circuit(ast::Module module, Implementation &impl)
		    : module(module), _state(module.state_size()), _outputs(module.output_size()),
		      impl(impl) {
			impl.initialize(_state);
		}

		void evaluate(const std::vector<T> &inputs);

		const std::vector<T> &state() const { return _state; };
		const std::vector<T> &outputs() const { return _outputs; };
	};
};

#include "circuit.hpp"
#include "expression.hpp"
