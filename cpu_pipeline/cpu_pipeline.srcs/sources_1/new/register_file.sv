module register_file (
    input logic clk,
    input logic reset,

    input logic [2:0] read_addr1, // Rs1
    input logic [2:0] read_addr2, // Rs2
    output logic [7:0] read_data1,
    output logic [7:0] read_data2,

    input  logic [2:0] write_addr, // Rd
    input  logic [7:0] write_data,
    input  logic write_enable
);

    logic [7:0] registers [0:7];
    
    assign read_data1 = registers[read_addr1];
    assign read_data2 = registers[read_addr2];

    always_ff @(posedge clk) begin
        if (reset) begin
            for (int i = 0; i < 8; i++) registers[i] <= 8'b0;
        end
        else if (write_enable) registers[write_addr] <= write_data;
    end

endmodule