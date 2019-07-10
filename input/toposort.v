// Tests topological sorting with a messy assignment graph
module TOPOSORT (
	clk
	input a, b, c, d
	output x1, x2, x3, x4
);
	assign x2 = x1
	assign x3 = x4 OR x1
	assign x4 = b AND c
	assign x1 = FF1
	FF1 = a OR d OR x4 OR x2
endmodule
