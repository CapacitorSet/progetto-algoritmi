#pragma once

#include "utils.h"
#include <optional>
#include <variant>
#include <vector>

namespace ast {
	enum class Operator { NOT, AND, OR, XOR, NAND, NOR, XNOR };
	std::optional<Operator> try_resolve_operator(std::string);
	uint8_t arity(Operator);

	struct Input {
		size_t offset; // Offset into the inputs array
		inline bool operator==(const Input &other) const { return offset == other.offset; }
	};
	struct Output {
		size_t offset; // Offset into the inputs array
		inline bool operator==(const Output &other) const { return offset == other.offset; }
	};
	struct Flipflop {
		size_t offset; // Offset into the inputs array
		inline bool operator==(const Flipflop &other) const { return offset == other.offset; }
	};

	// Represents the left side of an assignment
	using LValue = std::variant<Output, Flipflop>;

	using Token = std::variant<Operator, Input, Output, Flipflop>;

	// Just syntactic sugar. Implemented with #defines to work with arbitrary variants.
#define is_input(token) std::holds_alternative<ast::Input>(token)
#define is_operator(token) std::holds_alternative<ast::Operator>(token)
#define is_ff(token) std::holds_alternative<ast::Flipflop>(token)
#define is_output(token) std::holds_alternative<ast::Output>(token)
#define get_input(token) std::get<ast::Input>(token)
#define get_ff(token) std::get<ast::Flipflop>(token)
#define get_operator(token) std::get<ast::Operator>(token)
#define get_output(token) std::get<ast::Output>(token)

	using Expression = std::deque<Token>;

	struct Assignment {
		LValue lvalue;
		Expression expression;
		Assignment(const ast::LValue &lvalue, const Expression &expr)
		    : lvalue(lvalue), expression(expr){};
	};

	class Module {
	  public:
		bool isClocked;

		// Used when printing information to the user.
		std::vector<std::string> input_names;
		std::vector<uint16_t> flipflop_ids;
		std::vector<std::string> output_names;

		std::vector<Assignment> assignments;

		Module() = default;
		Module(bool isClocked, std::vector<std::string> input_names,
		       std::vector<uint16_t> flipflop_ids, std::vector<std::string> output_names,
		       std::vector<Assignment> assignments)
		    : isClocked(isClocked), input_names(input_names), flipflop_ids(flipflop_ids),
		      output_names(output_names), assignments(assignments) {}

		std::string name_of(Input) const;
		std::string name_of(Flipflop) const;
		std::string name_of(Output) const;
		std::string name_of(Operator) const;
		std::string name_of(Token) const;

		size_t input_size() const { return input_names.size(); }
		size_t state_size() const { return flipflop_ids.size(); }
		size_t output_size() const { return output_names.size(); }
	};
} // namespace ast

// Implement std::hash for AST types so that they can be used as indices in sets, maps
namespace std {
	template <>
	struct hash<ast::Input> {
		std::size_t operator()(const ast::Input &token) const { return token.offset; }
	};
	template <>
	struct hash<ast::Output> {
		std::size_t operator()(const ast::Output &token) const { return token.offset; }
	};
	template <>
	struct hash<ast::Flipflop> {
		std::size_t operator()(const ast::Flipflop &token) const { return token.offset; }
	};
} // namespace std
