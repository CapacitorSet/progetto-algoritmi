#include "ast.h"

using namespace ast;

std::string Module::name_of(Input i) const { return input_names[i.offset]; }
std::string Module::name_of(Output o) const { return output_names[o.offset]; }
std::string Module::name_of(Flipflop ff) const {
	return "FF" + std::to_string(flipflop_ids[ff.offset]);
}
std::string Module::name_of(Operator op) const {
	switch (op) {
		case Operator::NOT:
			return "NOT";
		case Operator::AND:
			return "AND";
		case Operator::OR:
			return "OR";
		case Operator::XOR:
			return "XOR";
		case Operator::NAND:
			return "NAND";
		case Operator::NOR:
			return "NOR";
		case Operator::XNOR:
			return "XNOR";
	}
}

std::string Module::name_of(Token t) const {
	return std::visit([&](auto &&token) { return name_of(token); }, t);
}

std::optional<Operator> ast::try_resolve_operator(std::string name) {
	if (name == "NOT")
		return Operator::NOT;
	else if (name == "AND")
		return Operator::AND;
	else if (name == "OR")
		return Operator::OR;
	else if (name == "XOR")
		return Operator::XOR;
	else if (name == "NAND")
		return Operator::NAND;
	else if (name == "NOR")
		return Operator::NOR;
	else if (name == "XNOR")
		return Operator::XNOR;
	else
		return {};
}

uint8_t ast::arity(Operator op) {
	switch (op) {
		case Operator::NOT:
			return 1;
		case Operator::AND:
		case Operator::OR:
		case Operator::XOR:
		case Operator::NAND:
		case Operator::NOR:
		case Operator::XNOR:
			return 2;
	}
}
