# 8-bit CPU Emulator

A custom 8-bit CPU designed and built from scratch in C++. An original architecture (not a RISC-V or ARM implementation), designed to understand the reasoning behind real CPU design decisions. A SystemVerilog/FPGA hardware port of a subset of the same ISA, implemented as a pipelined datapath on a Basys 3 board, demonstrates the same architecture at both the software and hardware level.

## Features

- Custom 16-bit fixed-length instruction set: 13 instructions across 4 formats (R-type, I-type, Indirect-type, Jump-type)
- 64KB byte-addressable, Von Neumann memory model
- Full fetch-decode-execute cycle, with a 4-bit flags register (Z/N/C/V) correctly implementing signed comparison (N XOR V) for ordering jumps
- A two-pass assembler that translates human-readable assembly (with labels and comments) into machine code, resolving forward and backward jumps
- Three validated test programs, each run both by direct hand-encoding and through the assembler, with identical, verified output: **Factorial** (0! to 5!, nested loops, multiplication using repeated addition), **Fibonacci** (fib(0) to fib(13), with all values stored), **Bubble sort** (computed runtime addressing, comparison and swap logic)

## Design

Every architectural decision (register count, instruction formats, why certain instructions were excluded, addressing mode trade-offs, and the assembler's design) is documented with reasoning in [ISA.md](ISA.md), including a running log of over 20 decisions, each paired with the constraint that forced it and the alternative considered.

## FPGA port

A subset of the ISA (ADD, SUB, LOADI, ADDI, HALT) is implemented in SystemVerilog as a 4-stage pipelined datapath (IF/ID/EX/WB) with data forwarding, targeting a Basys 3 FPGA. Source files, the testbench and the constraints file are in `cpu_pipeline/`.

- Timing met at the 100 MHz board clock with zero violations post-implementation (+4.6 ns worst setup slack, +0.173 ns worst hold slack); estimated maximum clock speed of about 186 MHz, extrapolated from the slack
- 98 LUTs and 126 flip-flops
- Functional equivalence and data hazard forwarding verified against the C++ emulator through a self-checking testbench

## Build & run

`make` then `make run`. Requires a C++17 compiler (g++). `main.cpp` runs a selected test suite or program by default. See the comments at the top of `main()` to switch between the instruction-level tests, the three hand-encoded programs, or the three assembled programs. To assemble and run a program directly: `readAsmFile(...)` → `assembleProgram(...)` → `writeBinFile(...)` → `loadBinFile(...)` → `cpu.reset(0x0100)` → `cpu.run()`.

The FPGA design is a Vivado project: open `cpu_pipeline/cpu_pipeline.xpr`, run the simulation with `cpu_pipeline_tb.sv`, or generate a bitstream for the Basys 3 board.

## Project structure

`include/` (header files), `src/` (implementation files), `programs/` (test programs: `.asm` source and assembled `.bin` output), `cpu_pipeline/` (Vivado project: SystemVerilog sources, testbench and constraints), `ISA.md` (full ISA design and decision log).

## Status

Complete: CPU emulator, assembler, and all three test programs implemented and verified. FPGA port of a subset of the ISA implemented, simulated, and run on a Basys 3 board.