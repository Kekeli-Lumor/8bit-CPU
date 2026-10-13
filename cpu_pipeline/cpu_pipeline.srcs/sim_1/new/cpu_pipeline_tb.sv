`timescale 1ns / 1ps

module cpu_pipeline_tb;

    logic clk;
    logic reset;
    logic [7:0] r0_snap, r7_snap;

    cpu_pipeline test_cpu (.clk(clk), .reset(reset));

    // Generate a clock: toggles every 5ns, giving a 10ns period (100MHz simulated)
    always #5 clk = ~clk;

    initial begin
        clk = 0;
        reset = 1;
        #10; // hold reset for one full clock period
        reset = 0;

        // Let the pipeline run long enough for the test program to complete.
        // 5 instructions, 4-stage pipeline: allow generous margin for fill + drain.
        #200;

        // Directly inspect the register file's internal contents through the
        // hierarchy for verification - testbench-only visibility, not
        // something real hardware/software could normally do.
        $display("R0=%d R1=%d R2=%d R3=%d R4=%d R5=%d R6=%d R7=%d halted=%d", test_cpu.rf.registers[0], test_cpu.rf.registers[1], test_cpu.rf.registers[2],
            test_cpu.rf.registers[3], test_cpu.rf.registers[4], test_cpu.rf.registers[5], test_cpu.rf.registers[6], test_cpu.rf.registers[7], test_cpu.halted);

        if (test_cpu.rf.registers[0] === 8'd9  && test_cpu.rf.registers[1] === 8'd10 && test_cpu.rf.registers[2] === 8'd24 && test_cpu.rf.registers[3] === 8'd15 &&
            test_cpu.rf.registers[4] === 8'd8  && test_cpu.rf.registers[5] === 8'd10 && test_cpu.rf.registers[6] === 8'd1  && test_cpu.rf.registers[7] === 8'd0 && test_cpu.halted === 1'b1)
            $display("PASS: SUB, WB-hazard forwarding, and HALT all correct");
        else
            $display("FAIL: check results above");



        r0_snap = test_cpu.rf.registers[0];
        r7_snap = test_cpu.rf.registers[7];
        #200; // let the halted CPU run for many more cycles
        if (test_cpu.rf.registers[0] === r0_snap && test_cpu.rf.registers[7] === r7_snap)
            $display("PASS: registers stable after HALT");
        else
            $display("FAIL: registers changed after HALT (R0=%d R7=%d)", test_cpu.rf.registers[0], test_cpu.rf.registers[7]);
             
        $finish;
    end

endmodule