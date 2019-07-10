#pragma once

#include "generic.hpp"
#include <forward_list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace analysis {
	using Token = std::variant<ast::Operator, ast::Input, ast::Flipflop>;

	struct Node {
		Token token;
		std::array<Node *, 2> children;
	};

	class Implementation;
	using Engine = GenericSimulator<Node, Implementation>;

	class Implementation {
		/* Linked lists are the only data structures that allow us to use pointers to nodes. Other
		 * containers (eg. vectors) could invalidate them.
		 */
		std::forward_list<Node> nodes;

		Node *fetch_operand(Engine::OperandStack &node);

	  public:
		Implementation() = default;
		static void initialize(std::vector<Node> &state);
		void on_operator(ast::Operator, Engine::OperandStack &stack);
	};

	using StackMachine = Engine::StackMachine;
	using Circuit = Engine::Circuit;

	struct Path {
		std::vector<Token> path;

		uint64_t length() const { return path.size(); };
		std::string toString(const ast::Module &module) const;
	};

	class GraphWalker {
		std::vector<Node> outputs;
		std::vector<Node> flipflops;
		std::deque<Token> current_path;
		std::deque<ast::LValue> current_lvalue_hierarchy;
		std::unordered_set<ast::Flipflop> visited_ffs;

	  public:
		// The set is ordered for aesthetic reasons.
		using LogicCone = std::set<size_t>;
		std::unordered_map<ast::LValue, LogicCone> logic_cones;
		Path longest_path, shortest_path;

		GraphWalker(const Circuit &ckt)
		    : outputs(ckt.outputs()), flipflops(ckt.state()), longest_path({}), shortest_path({}) {}
		void walk_output(ast::Output);

	  private:
		void walk(const Node &node);
		void process(ast::Input);
		void process(ast::Flipflop);
		void process(ast::Operator);
	};

	void run(const ast::Module &);

} // namespace analysis
