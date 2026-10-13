module instruction_memory (
    input logic [15:0] pc,
    output logic [15:0] instruction
);
    logic [15:0] rom [0:31];

    initial begin
        // Assign every ROM entry to HALT so unused entries are harmless
        for (int i = 0; i < 32; i++) rom[i] = {5'b00010, 11'b0};
    
        // Test program: 
        rom[0] = {5'b01000, 3'b000, 8'd9}; // LOADI R0, 9
        rom[1] = {5'b01000, 3'b001, 8'd10}; // LOADI R1, 10
        rom[2] = {5'b00000, 3'b010, 3'b000, 3'b001, 2'b00}; // ADD R2, R0, R1 (19)
        rom[3] = {5'b01001, 3'b010, 8'd5}; // ADDI R2, 5 (24)
        rom[4] = {5'b01000, 3'b011, 8'd15}; // LOADI R3, 15
        rom[5] = {5'b01000, 3'b100, 8'd8}; // LOADI R4, 8
        rom[6] = {5'b00001, 3'b101, 3'b011, 3'b100, 2'b00}; // SUB R5, R3, R4 (7)
        rom[7] = {5'b01000, 3'b110, 8'd1}; // LOADI R6, 1  (creates the 2-instruction gap)
        rom[8] = {5'b01001, 3'b101, 8'd3};    // ADDI R5, 3 (10) (WB-hazard, 2 back)
        rom[9] = {5'b00010, 11'b0}; // HALT
        rom[10] = {5'b01001, 3'b111, 8'd1};  // ADDI R7, 1 after HALT (shouldn't execute to prove halt was successful)
    end

    assign instruction = rom[pc[5:1]]; // word-addressed (pc increments by 2)
endmodule