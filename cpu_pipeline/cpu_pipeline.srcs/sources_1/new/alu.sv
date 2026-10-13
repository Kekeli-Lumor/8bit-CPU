module alu (
    input logic [7:0] a,
    input logic [7:0] b,
    input logic is_sub, // 0 = ADD, 1 = SUB

    output logic [7:0] result,
    output logic zero_flag,
    output logic negative_flag,
    output logic carry_flag,
    output logic overflow_flag
);

    logic [7:0] operand_b;
    logic carry_in;
    logic [8:0] widened_result; // 9 bits, so bit 8 captures carry-out

    // SUB is implemented as A + (~B + 1): invert B and add 1 as the adder's carry in.
    // Allows one adder to handle both operations.
    assign operand_b = is_sub ? ~b : b;
    assign carry_in = is_sub;
    assign widened_result = {1'b0, a} + {1'b0, operand_b} + {8'b0, carry_in};
    assign result = widened_result[7:0];

    assign zero_flag = (result == 8'b0);
    assign negative_flag = result[7];
    assign carry_flag = is_sub ? ~widened_result[8] : widened_result[8];

    // Overflow: a and operand_b (~b for SUB) share a sign, result's sign differs
    assign overflow_flag = (a[7] == operand_b[7]) && (result[7] != a[7]);

endmodule
