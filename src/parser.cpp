#include "parser.h"

#include <unordered_set>

using namespace ast;

// Split the line by whitespace
std::vector<std::string> FileParser::tokenize(const std::string &line) {
	std::string delimiters = " \t";
	std::vector<std::string> tokens;
	// For simplicity, basic comment support is implemented at the tokenizer level.
	if (line[0] == '/' && line[1] == '/')
		return {};
	size_t start = line.find_first_not_of(delimiters);

	while (start != std::string::npos) {
		// Find next whitespace
		size_t end = line.find_first_of(delimiters, start);
		tokens.push_back(line.substr(start, end - start));
		// Skip all whitespace
		start = line.find_first_not_of(delimiters, end);
	}
	return tokens;
}

bool FileParser::isValidFFName(const std::string &token) const {
	return token.length() > 2 && token.substr(0, 2) == "FF" &&
	       token.substr(2, token.length()).find_first_not_of("0123456789") == std::string::npos;
}

FileParser::FileParser(std::istream &stream) : state(State::IDLE), isClocked(false) {
	std::string line;
	for (uint64_t linenum = 0; std::getline(stream, line); linenum++) {
		std::vector<std::string> tokens = tokenize(line);
		try {
			for (const std::string &token : tokens)
				ingest(token);
			ingest_newline();
		} catch (std::string &e) {
			throw e + " [line " + std::to_string(linenum) + "]";
		}
	}
}

// Ensures that assignments are topologically sorted when parsing has finished
Module FileParser::finalize() {
	if (state != State::IDLE)
		throw "Parsing ended prematurely"s;
	std::vector<Assignment> sorted_assignments = toposort_assignments();
	return Module(isClocked, inputs, flipflops, outputs, sorted_assignments);
}

/* This method sorts assignments topologically using Kahn's algorithm. Note that children represent
 * operands.
 *
 * The possibility of loops involving flip-flops, which would normally be rejected
 * by the algorithm, requires careful consideration. The solution adopted here is to "split" the
 * flip-flop into an input node and an output node not connected between each other, reflecting the
 * fact that the flip-flop doesn't have a direct dependency between its input and its output.
 *
 * The output node is an orphan node and is added to the initial set, but must not be added to the
 * list of sorted nodes; rather, we must add it when all of the flip-flop inputs have been visited,
 * which is when the input node becomes an orphan.
 * FF.
 */
std::vector<Assignment> FileParser::toposort_assignments() const {
	using namespace ast;

	using Node = std::variant<Input, Output, Flipflop>;
	using LogicCone = std::unordered_set<Node>;

	// Calculate logic cones in advance to cut down on lookup costs in Kahn's algorithm
	std::unordered_map<LValue, LogicCone> logic_cones;
	for (const std::pair<const LValue, Expression> &assignment : assignments) {
		LogicCone logic_cone;
		for (const Token &token : assignment.second) {
			if (is_input(token))
				logic_cone.emplace(get_input(token));
			else if (is_ff(token))
				logic_cone.emplace(get_ff(token));
			else if (is_output(token))
				logic_cone.emplace(get_output(token));
		}
		logic_cones[assignment.first] = logic_cone;
	}

	std::vector<Assignment> sorted_assignments;
	std::stack<Node> childless_nodes;
	for (size_t i = 0; i < inputs.size(); i++)
		childless_nodes.push(Input{i});
	for (size_t i = 0; i < flipflops.size(); i++)
		childless_nodes.push(Flipflop{i});

	while (!childless_nodes.empty()) {
		Node node = pop(childless_nodes);

		// Find parents of `node`, i.e. expressions that depend on it
		for (std::pair<const LValue, LogicCone> &kv_pair : logic_cones) {
			const LValue &lvalue = kv_pair.first;
			LogicCone &logic_cone = kv_pair.second;
			auto pos = logic_cone.find(node);
			// If `lvalue` is indeed a parent...
			if (pos != logic_cone.end()) {
				// Remove the edge `lvalue->node`
				logic_cone.erase(pos);
				// If `lvalue` has become childless, push it to `childless_nodes`
				if (logic_cone.empty()) {
					// Note that we don't push FFs to `childless_nodes`: we already did that at the
					// beginning, and doing so again will create a loop.
					if (is_output(lvalue))
						childless_nodes.push(get_output(lvalue));

					// In Kahn's algorithm, this would be the step where we add `node` to the list
					// of sorted nodes. We push directly onto `sorted_assignments` instead.
					sorted_assignments.emplace_back(lvalue, assignments.at(lvalue));
				}
			}
		}
	}

	for (std::pair<const ast::LValue, LogicCone> &kv_pair : logic_cones) {
		if (!kv_pair.second.empty())
			throw "The circuit contains feedback loops"s;
	}

	return sorted_assignments;
}

void FileParser::ingest(const std::string &token) {
	switch (state) {
		case State::IDLE:
			if (token == "module")
				state = State::MODULE_DECLARATION;
			else
				throw "Unexpected token: \"" + token + "\"";
			break;
		case State::MODULE_DECLARATION:
			// The module name is unused in the assignment.
			state = State::PARAMETER_DECLARATION;
			break;
		case State::PARAMETER_DECLARATION: {
			std::string clean_token = token;
			if (token.front() == '(')
				clean_token.erase(0, 1);

			if (clean_token.empty())
				; // We ingested a simple open parenthesis, let's do nothing
			else if (clean_token == "clk")
				isClocked = true;
			else if (clean_token == "input")
				state = State::INPUT_PARAMETERS;
			else if (clean_token == "output")
				state = State::OUTPUT_PARAMETERS;
			else if (clean_token == ");")
				state = State::MODULE_BODY;
			else
				throw "Unexpected token: \"" + clean_token + "\"";
			break;
		}
		case State::INPUT_PARAMETERS: {
			std::string clean_token = token;
			if (token.back() == ',')
				clean_token.pop_back();

			bool input_exists = input_find(clean_token).has_value();
			bool output_exists = output_find(clean_token).has_value();
			if (input_exists || output_exists)
				throw "Input parameter \"" + clean_token + "\" was already declared";

			input_map[clean_token] = ast::Input{inputs.size()};
			inputs.push_back(clean_token);
			break;
		}
		case State::OUTPUT_PARAMETERS: {
			std::string clean_token = token;
			if (token.back() == ',')
				clean_token.pop_back();

			bool input_exists = input_find(clean_token).has_value();
			bool output_exists = output_find(clean_token).has_value();
			if (input_exists || output_exists)
				throw "Output parameter \"" + clean_token + "\" was already declared";

			output_map[clean_token] = ast::Output{outputs.size()};
			outputs.push_back(clean_token);
			break;
		}
		case State::MODULE_BODY:
			if (token == "endmodule")
				state = State::IDLE;
			else if (token == "assign")
				state = State::ASSIGNMENT_START;
			else if (isValidFFName(token)) {
				if (!isClocked)
					throw "Flip-flop found in asynchronous circuit"s;
				uint16_t id = std::stoi(token.substr(2, token.length()));
				temporaryFFAssignment.lvalue = find_or_create_ff_id(id);
				temporaryFFAssignment.parser = ExpressionParser();
				state = State::FF_ASSIGNMENT_EQUALS;
			} else
				throw "Unexpected token: \"" + token + "\"";
			break;
		case State::ASSIGNMENT_START:
			if (!output_find(token).has_value())
				throw "No such output: " + token;
			temporaryAssignment.lvalue = ast::Output{output_map[token]};
			temporaryAssignment.parser = ExpressionParser();
			state = State::ASSIGNMENT_EQUALS;
			break;
		case State::ASSIGNMENT_EQUALS:
			if (token == "=")
				state = State::ASSIGNMENT_BODY;
			else
				throw "Unexpected token: \"" + token + "\"";
			break;
		case State::ASSIGNMENT_BODY:
			try {
				temporaryAssignment.parser.ingest(token);
			} catch (std::string &e) {
				throw "An error occurred while parsing the expression: " + e;
			}
			break;
		case State::FF_ASSIGNMENT_EQUALS:
			if (token == "=")
				state = State::FF_ASSIGNMENT_BODY;
			else
				throw "Unexpected token: \"" + token + "\"";
			break;
		case State::FF_ASSIGNMENT_BODY:
			try {
				temporaryFFAssignment.parser.ingest(token);
			} catch (std::string &e) {
				throw "An error occurred while parsing the expression: " + e;
			}
			break;
	}
}

void FileParser::ingest_newline() {
	switch (state) {
		case State::INPUT_PARAMETERS:
		case State::OUTPUT_PARAMETERS:
			state = State::PARAMETER_DECLARATION;
			break;
		case State::ASSIGNMENT_BODY: {
			try {
				std::deque<std::string> assignment = temporaryAssignment.parser.finalize();
				Expression expression = compile(assignment);
				assignments[temporaryAssignment.lvalue] = expression;
			} catch (std::string &e) {
				throw "An error occurred while parsing the expression: " + e;
			}
			state = State::MODULE_BODY;
			break;
		}
		case State::FF_ASSIGNMENT_BODY: {
			try {
				std::deque<std::string> assignment = temporaryFFAssignment.parser.finalize();
				Expression expression = compile(assignment);
				assignments[temporaryFFAssignment.lvalue] = expression;
			} catch (std::string &e) {
				throw "An error occurred while parsing the expression: " + e;
			}
			state = State::MODULE_BODY;
			break;
		}
	}
}

// Compiles an assignment made of tokens into a proper ast::Expression
Expression FileParser::compile(const std::deque<std::string> &assignment) {
	Expression ret;
	for (const std::string &token : assignment) {
		std::optional<Operator> op = try_resolve_operator(token);
		if (op.has_value())
			ret.push_back(op.value());
		else if (isValidFFName(token)) {
			uint16_t id = std::stoi(token.substr(2, token.length()));
			ret.push_back(find_or_create_ff_id(id));
		} else {
			std::optional<Input> input = input_find(token);
			std::optional<Output> output = output_find(token);
			if (input.has_value())
				ret.push_back(input.value());
			else if (output.has_value())
				ret.push_back(output.value());
			else
				throw "No such variable: " + token;
		}
	}
	return ret;
}

std::optional<Input> FileParser::input_find(const std::string &input) const {
	auto it = input_map.find(input);
	if (it == input_map.end())
		return {};
	else
		return it->second;
}

std::optional<Output> FileParser::output_find(const std::string &input) const {
	auto it = output_map.find(input);
	if (it == output_map.end())
		return {};
	else
		return it->second;
}

Flipflop FileParser::find_or_create_ff_id(uint16_t id) {
	auto it = ff_map.find(id);
	if (it == ff_map.end()) {
		flipflops.push_back(id);
		ff_map[id] = ast::Flipflop{flipflops.size() - 1};
		size_t idx = flipflops.size() - 1;
		return Flipflop{idx};
	} else {
		return it->second;
	}
}

// Implements Dijkstra's shunting yard algorithm
void ExpressionParser::ingest(const std::string &token) {
	// Special cases for tokens that lump together a parenthesis
	if (token.length() > 1 && token.substr(0, 1) == "(") {
		ingest("(");
		ingest(token.substr(1, token.length() - 1));
	} else if (token.length() > 1 && token.back() == ')') {
		ingest(token.substr(0, token.length() - 1));
		ingest(")");
	} else if (token == "(") {
		operatorStack.push(token);
	} else if (token == ")") {
		while (!operatorStack.empty() && operatorStack.top() != "(") {
			output.push_front(operatorStack.top());
			operatorStack.pop();
		}
		if (operatorStack.empty())
			throw "Mismatched parentheses"s;
		// The top must contain a "(" by now.
		operatorStack.pop();
	} else {
		std::optional<Operator> op = try_resolve_operator(token);
		if (op.has_value()) {
			if (arity(op.value()) == 2)
				while (!operatorStack.empty() && operatorStack.top() != "(") {
					output.push_front(operatorStack.top());
					operatorStack.pop();
				}
			operatorStack.push(token);
		} else {
			output.push_front(token);
		}
	}
}

std::deque<std::string> ExpressionParser::finalize() {
	while (!operatorStack.empty()) {
		if (operatorStack.top() == "(")
			throw "Mismatched parentheses"s;
		output.push_front(operatorStack.top());
		operatorStack.pop();
	}
	return output;
}
