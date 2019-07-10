module LOGIC_PROPERTIES (
	clk
	input a, b, c, d
// Doeshat it says on the tin
	output always_true, always_false
// Will always equal a
	output double_negation
// Will always equal a
	output and_identity, or_identity
// Will always equal 1
	output all_operators_commutativity
// Will always equal 1
	output all_operators_associativity
// Will always equal 1
	output de_morgan_nand, de_morgan_nor
);
	assign always_true = a OR NOT a
	assign always_false = a AND NOT a

	assign double_negation = NOT NOT a
	assign and_identity = a AND always_true
	assign or_identity = a OR always_false

// Checks that (a OP b) == (b OP a) for all binary ops.
 	assign all_operators_commutativity = ((a AND b) XNOR (b AND a)) AND ((a OR b) XNOR (b OR a)) AND ((a XOR b) XNOR (b XOR a)) AND ((a NAND b) XNOR (b NAND a)) AND ((a NOR b) XNOR (b NOR a)) AND ((a XNOR b) XNOR (b XNOR a))
// Checks that (a OP (b OP c)) == ((a OP b) OP c) for all binary ops.
 	assign all_operators_associativity = ((a AND (b AND c)) XNOR ((a AND b) AND c)) AND ((c OR d) XNOR (d OR c)) AND ((c XOR d) XNOR (d XOR c)) AND ((c NAND d) XNOR (d NAND c)) AND ((c NOR d) XNOR (d NOR c)) AND ((c XNOR d) XNOR (d XNOR c))

 	assign de_morgan_nand = ((NOT a) AND (NOT b)) XNOR (a NOR b)
 	assign de_morgan_nor = ((NOT a) OR (NOT b)) XNOR (a NAND b)
endmodule