#include "simulation.h"
#include <fstream>
#include <iostream>

void simulation::Implementation::on_operator(ast::Operator astOperator,
                                             simulation::Engine::OperandStack &stack) {
	switch (astOperator) {
		case ast::Operator::NOT:
			stack.push(!pop(stack));
			break;
		case ast::Operator::AND:
			stack.push(pop(stack) && pop(stack));
			break;
		case ast::Operator::OR:
			stack.push(pop(stack) || pop(stack));
			break;
		case ast::Operator::XOR:
			stack.push(pop(stack) ^ pop(stack));
			break;
		case ast::Operator::NAND:
			stack.push(!(pop(stack) && pop(stack)));
			break;
		case ast::Operator::NOR:
			stack.push(!(pop(stack) || pop(stack)));
			break;
		case ast::Operator::XNOR:
			stack.push(!(pop(stack) ^ pop(stack)));
			break;
	}
}

void simulation::Implementation::initialize(std::vector<TruthValue> &state) {
	// The state of the system is initially indeterminate.
	std::fill(state.begin(), state.end(), TruthValue::X);
}

void simulation::run(const ast::Module &module) {
	std::cout << "Enter the path to the input vectors file (default: input/vectors.txt): ";
	std::cin.ignore(); // Skip the newline that's left in the buffer
	std::string input_filename;
	std::getline(std::cin, input_filename);
	if (input_filename.empty())
		input_filename = "input/vectors.txt";

	std::ifstream vectors_file(input_filename, std::ios::in);
	if (vectors_file.fail())
		throw "Failed to open file."s;

	std::cout << "Enter the path to the output file (default: console output): ";
	std::string output_filename;
	std::getline(std::cin, output_filename);
	std::ofstream output_file;
	// The pointer is used to get around the fact that you can't copy ostreams
	// A reference would be better, but it would also require a mess with ternary operators
	std::ostream *out;
	if (output_filename.empty()) {
		out = &std::cout;
	} else {
		output_file.open(output_filename);
		if (output_file.fail())
			throw "Failed to open file."s;
		out = &output_file;
	}

	simulation::Implementation impl;
	simulation::Circuit ckt(module, impl);

	std::string line;
	for (uint32_t linenum = 0; std::getline(vectors_file, line); linenum++) {
		if (line.size() != module.input_size())
			throw "Input size mismatch (line " + std::to_string(linenum) + ")";
		std::vector<TruthValue> inputs(module.input_size());
		for (size_t i = 0; i < module.input_size(); i++)
			inputs[i] = line[i] - '0';

		ckt.evaluate(inputs);

		for (const TruthValue &bit : ckt.outputs())
			*out << bit.toChar();
		*out << std::endl;
	}
}