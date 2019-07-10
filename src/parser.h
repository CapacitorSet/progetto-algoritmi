#pragma once

#include "ast.h"
#include <istream>
#include <unordered_map>

class ExpressionParser {
	std::deque<std::string> output;
	std::stack<std::string> operatorStack;

  public:
	ExpressionParser() = default;
	void ingest(const std::string &token);
	std::deque<std::string> finalize();
};

class FileParser {
	enum class State {
		IDLE,
		MODULE_DECLARATION,
		PARAMETER_DECLARATION,
		INPUT_PARAMETERS,
		OUTPUT_PARAMETERS,
		MODULE_BODY,
		ASSIGNMENT_START,
		ASSIGNMENT_EQUALS,
		ASSIGNMENT_BODY,
		FF_ASSIGNMENT_EQUALS,
		FF_ASSIGNMENT_BODY,
	};
	State state;

	bool isClocked;
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;
	std::vector<uint16_t> flipflops;

	// Auxiliary data structures for O(1) resolution of variable names
	std::unordered_map<std::string, ast::Input> input_map;
	std::unordered_map<std::string, ast::Output> output_map;
	std::unordered_map<uint16_t, ast::Flipflop> ff_map;

	// We don't care about order at this stage (pre-toposort)
	std::unordered_map<ast::LValue, ast::Expression> assignments;

	struct {
		ast::Output lvalue;
		ExpressionParser parser;
	} temporaryAssignment;
	struct {
		ast::Flipflop lvalue;
		ExpressionParser parser;
	} temporaryFFAssignment;

	void ingest(const std::string &token);
	void ingest_newline();

	// Helpers
	std::optional<ast::Input> input_find(const std::string &) const;
	std::optional<ast::Output> output_find(const std::string &) const;
	ast::Flipflop find_or_create_ff_id(uint16_t id);

	bool isValidFFName(const std::string &) const;

	static std::vector<std::string> tokenize(const std::string &line);
	ast::Expression compile(const std::deque<std::string> &assignment);
	std::vector<ast::Assignment> toposort_assignments() const;

  public:
	FileParser(std::istream &);
	ast::Module finalize();
};
