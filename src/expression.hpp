template <typename T, class Implementation>
T GenericSimulator<T, Implementation>::StackMachine::evaluate(const std::vector<T> &inputs,
                                                              const std::vector<T> &state,
                                                              const std::vector<T> &outputs) {
	while (!expression.empty()) {
		ast::Token token = pop_back(expression);
		if (is_input(token)) {
			size_t index = get_input(token).offset;
			operandStack.push(inputs[index]);
		} else if (is_ff(token)) {
			size_t index = get_ff(token).offset;
			operandStack.push(state[index]);
		} else if (is_operator(token)) {
			auto astOperator = get_operator(token);
			impl.on_operator(astOperator, operandStack);
		} else if (is_output(token)) {
			size_t index = get_output(token).offset;
			operandStack.push(outputs[index]);
		}
	}
	if (operandStack.empty())
		throw "The operand stack is empty"s;
	if (operandStack.size() > 1)
		throw "The operand stack contains more than one item"s;
	return operandStack.top();
}