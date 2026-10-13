module cpu_pipeline (
    input logic clk,
    input logic reset,
    output logic [2:0] debug_wb_addr,
    output logic [7:0] debug_wb_data,
    output logic debug_wb_enable,
    output logic debug_halted
    
    
);
    logic [15:0] pc;

    // Instruction Fetch (IF) Stage. Fetches instruction from memory and increments PC
    logic [15:0] if_instruction;
    instruction_memory imem (.pc(pc), .instruction(if_instruction));

    logic [15:0] if_id_instruction;
    always_ff @(posedge clk) begin
        if (reset) if_id_instruction <= 16'b0;
        else if_id_instruction <= if_instruction;
    end
    
    // Halts PC if HALT instruction is decoded by freezing it on the same cycle
    logic halted;
    always_ff @(posedge clk) begin
        if (reset) halted <= 1'b0;
        else if (is_halt) halted <= 1'b1;
    end
    
    always_ff @(posedge clk) begin
        if (reset) pc <= 16'b0;
        else if (halted || is_halt) pc <= pc;  // freeze same cycle HALT is decoded
        else pc <= pc + 16'd2;
    end

    // Instruction Decode (ID) Stage. Reads register file and decoeds opcode
    logic [4:0] opcode;
    logic [2:0] dest, src1, src2, rs1_addr;
    logic [7:0] imm;
    logic is_add, is_sub, is_addi, is_loadi, is_halt;
    
    always_comb begin
        opcode = if_id_instruction[15:11];
        dest = if_id_instruction[10:8];
        src1 = if_id_instruction[7:5];
        src2 = if_id_instruction[4:2];
        imm = if_id_instruction[7:0];
    
        is_addi = (opcode == 5'b01001); 
        is_loadi = (opcode == 5'b01000);
        is_add = (opcode == 5'b00000);
        is_sub = (opcode == 5'b00001);
        is_halt = (opcode == 5'b00010);

        rs1_addr = is_addi ? dest : src1; // ADDI: destination register ia also used as source
    
    end
    
    
    logic [7:0] reg_read1, reg_read2;

    logic [2:0] wb_addr;
    logic [7:0] wb_data;
    logic wb_enable;

    register_file rf ( .clk(clk), .reset(reset), .read_addr1(rs1_addr), .read_addr2(src2), .read_data1(reg_read1), .read_data2(reg_read2),
    .write_addr(wb_addr), .write_data(wb_data), .write_enable(wb_enable) );
    
    // Forwarding logic: resolve data hazards before ID/EX capture 
    logic [7:0] forwarded_read1, forwarded_read2;

    always_comb begin
        forwarded_read1 = reg_read1;
        forwarded_read2 = reg_read2;

        if (ex_wb_reg_write && (ex_wb_dest == rs1_addr)) begin
            forwarded_read1 = ex_wb_result;
        end
        if (ex_wb_reg_write && (ex_wb_dest == src2)) begin
            forwarded_read2 = ex_wb_result;
        end

        if (id_ex_reg_write && (id_ex_dest == rs1_addr)) begin
            forwarded_read1 = ex_result;
        end
        if (id_ex_reg_write && (id_ex_dest == src2)) begin
            forwarded_read2 = ex_result;
        end
    end


    // ID/EX pipeline register
    logic [7:0] id_ex_a, id_ex_b;
    logic [2:0] id_ex_dest;
    logic id_ex_is_sub, id_ex_reg_write, id_ex_halt;

    always_ff @(posedge clk) begin
        if (reset) begin
            id_ex_reg_write <= 1'b0;
            id_ex_halt <= 1'b0;
        end else begin
            id_ex_a <= is_loadi ? 8'b0 : forwarded_read1;
            id_ex_b <= (is_loadi || is_addi) ? imm : forwarded_read2;
            id_ex_dest <= dest;
            id_ex_is_sub <= is_sub;
            id_ex_reg_write <= (is_add || is_sub || is_addi || is_loadi) && !halted;
            id_ex_halt <= is_halt;
        end
    end

    // Execute (EX) Stage. Performs arithmetic or logical operations using ALU
    logic [7:0] ex_result;
    logic ex_z, ex_n, ex_c, ex_v;

    alu alu_inst ( .a(id_ex_a), .b(id_ex_b), .is_sub(id_ex_is_sub), .result(ex_result), .zero_flag(ex_z), .negative_flag(ex_n),
    .carry_flag(ex_c), .overflow_flag(ex_v) );

    // EX/WB pipeline register
    logic [7:0] ex_wb_result;
    logic [2:0] ex_wb_dest;
    logic ex_wb_reg_write;

    always_ff @(posedge clk) begin
        if (reset) ex_wb_reg_write <= 1'b0;
        else begin
            ex_wb_result <= ex_result;
            ex_wb_dest <= id_ex_dest;
            ex_wb_reg_write <= id_ex_reg_write;
        end
    end

    // Writeback (WB) Stage. Writes result back to register file
    assign wb_addr = ex_wb_dest;
    assign wb_data = ex_wb_result;
    assign wb_enable = ex_wb_reg_write;
    
    // Debug/monitor outputs are used to expose WB-stage activity and halt status so synthesis has real outputs to preserve logic against. 
    // Without these, the whole design has no observable effect and gets optimized away entirely, since it currently has no output ports at all).
    
    // Debug output is latched so that it only updates on a genuine register write, so it correctly holds the last real result
    logic [2:0] debug_wb_addr_reg;
    logic [7:0] debug_wb_data_reg;

    always_ff @(posedge clk) begin
        if (reset) begin
            debug_wb_addr_reg <= 3'b0;
            debug_wb_data_reg <= 8'b0;
        end else if (wb_enable) begin
            debug_wb_addr_reg <= wb_addr;
            debug_wb_data_reg <= wb_data;
        end
    end

    assign debug_wb_addr = debug_wb_addr_reg;
    assign debug_wb_data = debug_wb_data_reg;
    assign debug_wb_enable = wb_enable;
    assign debug_halted = halted;

endmodule
