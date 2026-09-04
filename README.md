# 8-bit CPU Emulator

A custom 8-bit CPU designed and built from scratch in C++. An original architecture (not a RISC-V or ARM implementation), designed  to understand the reasoning behind real CPU design decisions. A SystemVerilog/FPGA hardware port of the same ISA is in progress as a follow-up project, to demonstrate the same architecture at both the software and hardware level.

## Features

- Custom 16-bit fixed-length instruction set: 13 instructions across 4 formats (R-type, I-type, Indirect-type, Jump-type)
- 64KB byte-addressable, Von Neumann memory model
- Full fetch-decode-execute cycle, with a 4-bit flags register (Z/N/C/V) correctly implementing signed comparison (N XOR V) for ordering jumps
- A two-pass assembler that translates human-readable assembly (with labels and comments) into machine code, resolving forward and backward jumps
- Three validated test programs, each run both by direct hand-encoding and through the assembler, with identical, verified output: **Factorial** (0! to 5!, nested loops, multiplication using repeated addition), **Fibonacci** (fib(0) to fib(13), with all values being stored), **Bubble sort** (computed runtime addressing, comparison and swap logic)

## Design

Every architectural decision (register count, instruction formats, why certain instructions were excluded, addressing mode trade-offs, and the assembler's design)  is documented with reasoning in [ISA.md](ISA.md), including a running log of over 20 decisions, each paired with the constraint that forced it and the alternative considered.

## Build & run

`make` then `make run`. Requires a C++17 compiler (g++). `main.cpp` runs a selected test suite or program by default. See the comments at the top of `main()` to switch between the instruction-level tests, the three hand-encoded programs, or the three assembled programs. To assemble and run a program directly: `readAsmFile(...)` → `assembleProgram(...)` → `writeBinFile(...)` → `loadBinFile(...)` → `cpu.reset(0x0100)` → `cpu.run()`.

## Project structure

`include/` (header files), `src/` (implementation files), `programs/` (test programs  `.asm` source and assembled `.bin` output), `ISA.md` (full ISA design and decision log).

## Status

Complete: CPU emulator, assembler, and all three test programs implemented and verified. FPGA/Verilog hardware port in progress as a follow-up project.