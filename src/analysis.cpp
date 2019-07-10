#include "analysis.h"
#include <iostream>

using namespace analysis;

// Pop an operand, push it into the list of nodes, and return a pointer to the node
Node *Implementation::fetch_operand(Engine::OperandStack &stack) {
	nodes.push_front(pop(stack));
	return &nodes.front();
}

// Initializes the circuit state to simple, childless flip flops
void Implementation::initialize(std::vector<Node> &state) {
	for (size_t i = 0; i < state.size(); i++)
		state[i] = {ast::Flipflop{i}, {}};
}

// Just creates a new node with the appropriate children
void Implementation::on_operator(ast::Operator astOperator, Engine::OperandStack &stack) {
	switch (arity(astOperator)) {
		case 1:
			stack.push(Node{astOperator, {fetch_operand(stack)}});
			break;
		case 2:
			stack.push(Node{astOperator, {fetch_operand(stack), fetch_operand(stack)}});
			break;
	}
}

std::string Path::toString(const ast::Module &module) const {
	std::string ret = std::visit([&](auto &&token) { return module.name_of(token); }, path[0]);
	for (size_t i = 1; i < path.size(); i++)
		ret += " -> " + std::visit([&](auto &&token) { return module.name_of(token); }, path[i]);
	return ret;
}

// Walk the graph starting from this output
void GraphWalker::walk_output(ast::Output output) {
	current_lvalue_hierarchy.emplace_back(output);
	walk(outputs[output.offset]);
	current_lvalue_hierarchy.pop_back();
}

void GraphWalker::walk(const Node &node) {
	Token token = node.token;
	current_path.push_back(token);
	std::visit([&](auto &&token) { return this->process(token); }, token);
	for (Node *const child : node.children)
		if (child != nullptr)
			walk(*child);
	current_path.pop_back();
}

// Add this input to the relevant logic cones, and update the shortest/longest path if needed
void GraphWalker::process(ast::Input input) {
	for (const ast::LValue &item : current_lvalue_hierarchy)
		logic_cones[item].emplace(input.offset);
	uint16_t length = current_path.size();
	if (length > longest_path.length())
		longest_path.path = std::vector(current_path.begin(), current_path.end());
	if (length < shortest_path.length() || shortest_path.length() == 0)
		shortest_path.path = std::vector(current_path.begin(), current_path.end());
}

// Visit this FF if it hasn't been visited so far
void GraphWalker::process(ast::Flipflop ff) {
	bool ff_was_visited = visited_ffs.find(ff) != visited_ffs.end();
	if (ff_was_visited) {
		LogicCone logic_cone = logic_cones[ff];
		for (const ast::LValue &item : current_lvalue_hierarchy)
			logic_cones[item].merge(logic_cone);
	} else {
		visited_ffs.emplace(ff);
		current_lvalue_hierarchy.emplace_back(ff);
		walk(flipflops[ff.offset]);
		current_lvalue_hierarchy.pop_back();
	}
}

void GraphWalker::process(ast::Operator) { ; }

void analysis::run(const ast::Module &module) {
	std::vector<Node> inputs;
	// The i-th input is just a childless wrapper around the ast::Input token
	for (size_t i = 0; i < module.input_size(); i++)
		inputs.push_back(Node{ast::Input{i}, {}});

	Implementation impl;
	Circuit ckt(module, impl);
	ckt.evaluate(inputs);

	GraphWalker walker(ckt);
	for (size_t i = 0; i < ckt.outputs().size(); i++)
		walker.walk_output(ast::Output{i});

	std::cout << "Shortest path: " << walker.shortest_path.toString(module) << std::endl;
	std::cout << "Longest path: " << walker.longest_path.toString(module) << std::endl;
	for (size_t i = 0; i < module.output_size(); i++) {
		ast::Output output{i};
		std::cout << "Logic cone for " << module.name_of(output) << ":" << std::endl;
		for (const size_t &item_idx : walker.logic_cones[output])
			std::cout << "  - " << module.name_of(ast::Input{item_idx}) << std::endl;
	}
}