template <typename T, class Implementation>
void GenericSimulator<T, Implementation>::Circuit::evaluate(const std::vector<T> &inputs) {
	if (inputs.size() != module.input_size())
		throw "Input size mismatch"s;

	std::vector<T> state_buffer = _state;

	// Note that state is implicitly preserved across loops.
	for (const ast::Assignment &assignment : module.assignments) {
		ast::LValue lvalue = assignment.lvalue;
		StackMachine s(assignment.expression, impl);
		T result = StackMachine(assignment.expression, impl).evaluate(inputs, _state, _outputs);
		if (is_output(lvalue))
			_outputs[get_output(lvalue).offset] = result;
		else if (is_ff(lvalue))
			state_buffer[get_ff(lvalue).offset] = result;
	}

	// Emulate a clock tick: move the state buffer (FF inputs) into the state (FF values)
	_state = std::move(state_buffer);
}