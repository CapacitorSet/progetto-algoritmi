module single_gates (
        clk
		input a, b, c, d
		output _not, _and, _or, _xor, _nand, _nor, _xnor
	);
	assign _not = NOT a
	assign _and = a AND b
	assign _or = a OR b
	assign _xor = a XOR b
	assign _nand = a NAND b
	assign _nor = a NOR b
	assign _xnor = a XNOR b
endmodule
