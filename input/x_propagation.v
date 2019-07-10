// Tests that unitialized FFs and outputs have X values, and that X-values propagate correctly
module UNINITIALIZED (
	clk
	input a, b, c, d
// Uninitialized, will always be x
	output uninitialized_output
// Uninitialized, will always be x
	output uninitialized_ff
// Will be always be x
	output uninitialized_ff_child
// Will always be x
	output x_not
// Will be 0 for 0101, x otherwise
	output x_and
// Will be 1 for 0101, x otherwise
	output x_or
);
	assign uninitialized_ff = FF0
	assign x_not = NOT uninitialized_ff
	assign x_and = uninitialized_ff AND NOT (NOT a AND b AND NOT c AND d)
	assign x_or = uninitialized_ff OR (NOT a AND b AND NOT c AND d)
	
	FF1 = FF0
	assign uninitialized_ff_child = FF1
endmodule
