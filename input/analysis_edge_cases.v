// The name is self-explanatory - these are some edge cases, some of which
// actually caused problems during development
module ANALYSIS_EDGE_CASES (
	clk
	input a, b, c, d
	output tmp1, tmp2, tmp3
	output o1, o2, o3, o4, o5, o6
);
// Edge case: various forms of flip-flop feedback.
// We mustn't run into infinite loops when doing DFS!
	FF1 = FF1
	assign o1 = FF1

	assign o2 = FF2 OR a
	FF2 = o2 OR a

// Edge case: multiple input-output pairs with different depths
	assign o3 = b OR (NOT NOT NOT NOT NOT b)

// Edge case: logic cone propagation through multiple outputs
	assign tmp1 = a
	assign tmp2 = tmp1 OR b
	assign tmp3 = tmp2 OR c
	assign o4 = tmp3 OR d

// Edge case: logic cone propagation through multiple flip-flops
	FF3 = FF2 AND a
	FF4 = FF3
	FF5 = FF4
	assign o5 = FF5

// Edge case: logic cone propagation across cached flip-flops
	assign o6 = FF5
endmodule