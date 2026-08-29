#include <iostream>
#include <cstdint>

#include "cpu.hpp"
#include "memory.hpp"

//PROGRAM LOADING FUNCTIONS (loads program instructions into memory)

// Encodes and loads factorial program from 0! to 5! into memory, starting at 0x0100
void loadFactorialProgram(Memory& memory) {
    uint16_t addr = 0x0100;

    const uint8_t OP_ADD    = 0b00000;
    const uint8_t OP_SUB    = 0b00001;
    const uint8_t OP_HALT   = 0b00010;
    const uint8_t OP_MOV    = 0b00011;
    const uint8_t OP_LOADI  = 0b01000;
    const uint8_t OP_ADDI   = 0b01001;
    const uint8_t OP_STORE_ = 0b10001;
    const uint8_t OP_JMP    = 0b10010;
    const uint8_t OP_JLT    = 0b10101;

    // R0 is the outer loop counter (i)
    // R1 stores result. Is the running factorial value
    // R2 stores the memory address pointer (address where factorial results get written to)
    // R3 is the accumulator for multiplication
    // R4 is the inner loop counter (j)

    // 0: LOADI R1, 1: result = 1
    memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (1 & 0xFF));
    addr += 2;
    // 1: LOADI R2, 0: address pointer = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (2 << 8) | (0 & 0xFF));
    addr += 2;
    // 2: STORE R1, [R2+0]: memory[0] = 0! = 1
    memory.writeWord(addr, (OP_STORE_ << 11) | (1 << 8) | (2 << 5) | (0 & 0x1F));
    addr += 2;
    // 3: ADDI R2, 1: increment address pointer
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 4: LOADI R0, 1: i = 1
    memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (1 & 0xFF));
    addr += 2;

    // 5: start of outer loop: LOADI R5, 5
    memory.writeWord(addr, (OP_LOADI << 11) | (5 << 8) | (5 & 0xFF));
    addr += 2;
    // 6: SUB R6, R5, R0: R6 = 5 - i
    memory.writeWord(addr, (OP_SUB << 11) | (6 << 8) | (5 << 5) | (0 << 2));
    addr += 2;
    // 7: JLT to DONE (+12)
    memory.writeWord(addr, (OP_JLT << 11) | (12 & 0xFF));
    addr += 2;
    // 8: LOADI R3, 0: temp = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (0 & 0xFF));
    addr += 2;
    // 9: LOADI R4, 1: j = 1
    memory.writeWord(addr, (OP_LOADI << 11) | (4 << 8) | (1 & 0xFF));
    addr += 2;

    // 10: start of inner loop: SUB R6, R0, R4   ; R6 = i - j
    memory.writeWord(addr, (OP_SUB << 11) | (6 << 8) | (0 << 5) | (4 << 2));
    addr += 2;
    // 11: JLT inner loop done (+3)
    memory.writeWord(addr, (OP_JLT << 11) | (3 & 0xFF));
    addr += 2;
    // 12: ADD R3, R3, R1: temp += result
    memory.writeWord(addr, (OP_ADD << 11) | (3 << 8) | (3 << 5) | (1 << 2));
    addr += 2;
    // 13: ADDI R4, 1: j++
    memory.writeWord(addr, (OP_ADDI << 11) | (4 << 8) | (1 & 0xFF));
    addr += 2;
    // 14: JMP to start of inner loop (-5)
    memory.writeWord(addr, (OP_JMP << 11) | (static_cast<uint8_t>(-5) & 0xFF));
    addr += 2;

    // 15: end of inner loop: MOV R1, R3: result = temp
    memory.writeWord(addr, (OP_MOV << 11) | (1 << 8) | (3 << 5));
    addr += 2;
    // 16: STORE R1 [R2+0]
    memory.writeWord(addr, (OP_STORE_ << 11) | (1 << 8) | (2 << 5) | (0 & 0x1F));
    addr += 2;
    // 17: ADDI R2, 1: increment address pointer
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 18: ADDI R0, 1: increment i
    memory.writeWord(addr, (OP_ADDI << 11) | (0 << 8) | (1 & 0xFF));
    addr += 2;
    // 19: JMP to start of outer loop (-15)
    memory.writeWord(addr, (OP_JMP << 11) | (static_cast<uint8_t>(-15) & 0xFF));
    addr += 2;

    // 20: Done: HALT
    memory.writeWord(addr, (OP_HALT << 11));
    addr += 2;
}

// Encodes and loads fibonacci program from fib(0) to fib(13) into memory, starting at 0x0100
void loadFibonacciProgram(Memory& memory) {
    uint16_t addr = 0x0100;

    const uint8_t OP_ADD    = 0b00000;
    const uint8_t OP_SUB    = 0b00001;
    const uint8_t OP_HALT   = 0b00010;
    const uint8_t OP_MOV    = 0b00011;
    const uint8_t OP_LOADI  = 0b01000;
    const uint8_t OP_ADDI   = 0b01001;
    const uint8_t OP_STORE_ = 0b10001;
    const uint8_t OP_JMP    = 0b10010;
    const uint8_t OP_JLT    = 0b10101;

    // R0 is a, the running "previous" fibonacci value
    // R1 is b, the running "current" fibonacci value
    // R2 stores the memory address pointer
    // R3 is the loop counter (i)
    // R4 is temp, the next fibonacci value
    // R5 is the loop limit constant
    // R6 is a scratch register, used for the SUB comparison

    // 0: LOADI R0, 0: a = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (0 & 0xFF));
    addr += 2;
    // 1: LOADI R1, 1: b = 1
    memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (1 & 0xFF));
    addr += 2;
    // 2: LOADI R2, 0: address pointer = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (2 << 8) | (0 & 0xFF));
    addr += 2;
    // 3: STORE R0, [R2+0]: memory[0] = fib(0) = 0
    memory.writeWord(addr, (OP_STORE_ << 11) | (0 << 8) | (2 << 5) | (0 & 0x1F));
    addr += 2;
    // 4: ADDI R2, 1: increment address pointer
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 5: STORE R1, [R2+0]: memory[1] = fib(1) = 1
    memory.writeWord(addr, (OP_STORE_ << 11) | (1 << 8) | (2 << 5) | (0 & 0x1F));
    addr += 2;
    // 6: ADDI R2, 1: increment address pointer
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 7: LOADI R3, 2: i = 2
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (2 & 0xFF));
    addr += 2;

    // 8: start of loop: LOADI R5, 13
    memory.writeWord(addr, (OP_LOADI << 11) | (5 << 8) | (13 & 0xFF));
    addr += 2;
    // 9: SUB R6, R5, R3: R6 = 13 - i
    memory.writeWord(addr, (OP_SUB << 11) | (6 << 8) | (5 << 5) | (3 << 2));
    addr += 2;
    // 10: JLT to DONE (+7)
    memory.writeWord(addr, (OP_JLT << 11) | (7 & 0xFF));
    addr += 2;
    // 11: ADD R4, R0, R1: temp = a + b
    memory.writeWord(addr, (OP_ADD << 11) | (4 << 8) | (0 << 5) | (1 << 2));
    addr += 2;
    // 12: STORE R4, [R2+0]: memory[address pointer] = temp
    memory.writeWord(addr, (OP_STORE_ << 11) | (4 << 8) | (2 << 5) | (0 & 0x1F));
    addr += 2;
    // 13: ADDI R2, 1: increment address pointer
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 14: MOV R0, R1: a = b
    memory.writeWord(addr, (OP_MOV << 11) | (0 << 8) | (1 << 5));
    addr += 2;
    // 15: MOV R1, R4: b = temp
    memory.writeWord(addr, (OP_MOV << 11) | (1 << 8) | (4 << 5));
    addr += 2;
    // 16: ADDI R3, 1: increment i
    memory.writeWord(addr, (OP_ADDI << 11) | (3 << 8) | (1 & 0xFF));
    addr += 2;
    // 17: JMP to start of loop (-10)
    memory.writeWord(addr, (OP_JMP << 11) | (static_cast<uint8_t>(-10) & 0xFF));
    addr += 2;

    // 18: Done: HALT
    memory.writeWord(addr, (OP_HALT << 11));
    addr += 2;
}

// Encodes and loads a bubble sort program into memory, starting at 0x0100.
// Sorts a 5-element array ascending, in place, at mem[0..4].
void loadBubbleSortProgram(Memory& memory) {
    uint16_t addr = 0x0100;

    const uint8_t OP_ADD    = 0b00000;
    const uint8_t OP_SUB    = 0b00001;
    const uint8_t OP_HALT   = 0b00010;
    const uint8_t OP_LOADI  = 0b01000;
    const uint8_t OP_ADDI   = 0b01001;
    const uint8_t OP_LOAD_  = 0b10000;
    const uint8_t OP_STORE_ = 0b10001;
    const uint8_t OP_JMP    = 0b10010;
    const uint8_t OP_JLT    = 0b10101;

    // R0 is the array's starting address (constant, 0)
    // R1 is the outer loop counter (i)
    // R2 is the inner loop counter (j)
    // R3 is arr[j]
    // R4 is arr[j+1]
    // R5 is the loop bound constant (number of comparisons needed per pass - 1)
    // R6 is ptr, the computed address (base + j), refreshed each inner iteration
    // R7 is a scratch register, used for SUB comparisons

    // 0: LOADI R0, 0: base = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (0 & 0xFF));
    addr += 2;

    // Load the unsorted array into memory[0 to 4]: {15, 5, 7, 1, 3}
    // 1: LOADI R3, 15
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (15 & 0xFF));
    addr += 2;
    // 2: STORE R3, [R0+0]: mem[0] = 15
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (0 << 5) | (0 & 0x1F));
    addr += 2;
    // 3: LOADI R3, 5
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (5 & 0xFF));
    addr += 2;
    // 4: STORE R3, [R0+1]: mem[1] = 5
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (0 << 5) | (1 & 0x1F));
    addr += 2;
    // 5: LOADI R3, 7
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (7 & 0xFF));
    addr += 2;
    // 6: STORE R3, [R0+2]: mem[2] = 7
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (0 << 5) | (2 & 0x1F));
    addr += 2;
    // 7: LOADI R3, 1
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (1 & 0xFF));
    addr += 2;
    // 8: STORE R3, [R0+3]: mem[3] = 1
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (0 << 5) | (3 & 0x1F));
    addr += 2;
    // 9: LOADI R3, 3
    memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (3 & 0xFF));
    addr += 2;
    // 10: STORE R3, [R0+4]: mem[4] = 3
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (0 << 5) | (4 & 0x1F));
    addr += 2;

    // 11: LOADI R1, 0: i = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (0 & 0xFF));
    addr += 2;

    // 12: start of outer loop: LOADI R5, 3
    memory.writeWord(addr, (OP_LOADI << 11) | (5 << 8) | (3 & 0xFF));
    addr += 2;
    // 13: SUB R7, R5, R1: R7 = 3 - i
    memory.writeWord(addr, (OP_SUB << 11) | (7 << 8) | (5 << 5) | (1 << 2));
    addr += 2;
    // 14: JLT to outer loop done (+15)
    memory.writeWord(addr, (OP_JLT << 11) | (15 & 0xFF));
    addr += 2;
    // 15: LOADI R2, 0: j = 0
    memory.writeWord(addr, (OP_LOADI << 11) | (2 << 8) | (0 & 0xFF));
    addr += 2;

    // 16: start of inner loop: LOADI R5, 3
    memory.writeWord(addr, (OP_LOADI << 11) | (5 << 8) | (3 & 0xFF));
    addr += 2;
    // 17: SUB R7, R5, R2: R7 = 3 - j
    memory.writeWord(addr, (OP_SUB << 11) | (7 << 8) | (5 << 5) | (2 << 2));
    addr += 2;
    // 18: JLT to inner loop done (+9)
    memory.writeWord(addr, (OP_JLT << 11) | (9 & 0xFF));
    addr += 2;
    // 19: ADD R6, R0, R2: ptr = base + j
    memory.writeWord(addr, (OP_ADD << 11) | (6 << 8) | (0 << 5) | (2 << 2));
    addr += 2;
    // 20: LOAD R3, [R6+0]: arr[j]
    memory.writeWord(addr, (OP_LOAD_ << 11) | (3 << 8) | (6 << 5) | (0 & 0x1F));
    addr += 2;
    // 21: LOAD R4, [R6+1]: arr[j+1]
    memory.writeWord(addr, (OP_LOAD_ << 11) | (4 << 8) | (6 << 5) | (1 & 0x1F));
    addr += 2;
    // 22: SUB R7, R3, R4: R7 = arr[j] - arr[j+1]
    memory.writeWord(addr, (OP_SUB << 11) | (7 << 8) | (3 << 5) | (4 << 2));
    addr += 2;
    // 23: JLT to Skip swap (+2): if arr[j] < arr[j+1], already in order
    memory.writeWord(addr, (OP_JLT << 11) | (2 & 0xFF));
    addr += 2;
    // 24: STORE R4, [R6+0]: swap, arr[j] = old arr[j+1]
    memory.writeWord(addr, (OP_STORE_ << 11) | (4 << 8) | (6 << 5) | (0 & 0x1F));
    addr += 2;
    // 25: STORE R3, [R6+1]: arr[j+1] = old arr[j]
    memory.writeWord(addr, (OP_STORE_ << 11) | (3 << 8) | (6 << 5) | (1 & 0x1F));
    addr += 2;

    // 26: Skip swap: ADDI R2, 1: increment j
    memory.writeWord(addr, (OP_ADDI << 11) | (2 << 8) | (1 & 0xFF));
    addr += 2;
    // 27: JMP to start of inner loop (-12)
    memory.writeWord(addr, (OP_JMP << 11) | (static_cast<uint8_t>(-12) & 0xFF));
    addr += 2;

    // 28: inner loop done: ADDI R1, 1: increment i
    memory.writeWord(addr, (OP_ADDI << 11) | (1 << 8) | (1 & 0xFF));
    addr += 2;
    // 29: JMP to start of outer loop (-18)
    memory.writeWord(addr, (OP_JMP << 11) | (static_cast<uint8_t>(-18) & 0xFF));
    addr += 2;

    // 30: outer loop done: HALT
    memory.writeWord(addr, (OP_HALT << 11));
    addr += 2;
}




// INDIVIDUAL INSTRUCTION TESTS

void runInstructionTests(CPU& cpu, Memory& memory) {
    // Byte read and write test
    memory.writeByte(0x0010, 0xAB);
    uint8_t byteResult = memory.readByte(0x0010);
    std::cout << "Byte test: wrote 0x" << std::hex << static_cast<int>(byteResult) << "\n";
    std::cout << (byteResult == 0xAB ? "PASS" : "FAIL") << "\n";

    // Write word test. Confirm byte order is little endian
    memory.writeWord(0x0020, 0xCDAB);
    uint8_t lowByteResult = memory.readByte(0x0020);
    uint8_t highByteResult = memory.readByte(0x0021);
    std::cout << "Word write: wrote 0xCDAB. Value at 0x0020: 0x" << std::hex << static_cast<int>(lowByteResult) << " expecting 0x0ab. Value at 0x0021: 0x" << std::hex << static_cast<int>(highByteResult) << " expecting 0xcd \n";
    std::cout <<  ((lowByteResult == 0xAB && highByteResult == 0xCD) ? "PASS" : "FAIL") << "\n";

    // Write two bytes separately test. Confirm readWord reassembles them correctly
    memory.writeByte(0x0030, 0x34);
    memory.writeByte(0x0031, 0x12);
    uint16_t wordResult = memory.readWord(0x0030);
    std::cout << "Bytes written: 0x34 and 0x12. Value after readWord = 0x" << std::hex << wordResult << " (expecting 0x1234)" << "\n";
    std::cout << (wordResult == 0x1234 ? "PASS" : "FAIL") << "\n";
    std::cout << std::dec;

    // HALT test
    memory.writeWord(0x0100, 0b0001000000000000); // HALT opcode 00010 in bits [15:11]
    cpu.reset(0x0100);
    cpu.run();
    std::cout << (cpu.isHalted() ? "CPU halted correctly" : "CPU did NOT halt") << "\n";

    // LOAD and MOV test
    memory.writeWord(0x0000, 0b0100000000000001); // LOADI R0, imm=1
    memory.writeWord(0x0002, 0b0001100000000000); // MOV R1, R0
    memory.writeWord(0x0004, 0b0001000000000000); // HALT
    cpu.reset();
    cpu.run();
    std::cout << "R0 = " << (int)cpu.getRegister(0) << "\n";

    // SUB test: three tests, each in a fresh memory region
    // Built each instruction from its field rather than just using binary literals to make test easy to modify
    const uint8_t OP_SUB   = 0b00001;
    const uint8_t OP_HALT  = 0b00010;
    const uint8_t OP_LOADI = 0b01000;

    std::cout << "SUB test: \n";

    // Test 1: a = 5, b = 3. Intended results: result = 2, Z = 0, N = 0, C = 1, V = 0
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (5 & 0xFF));  addr += 2; // LOADI R0, 5
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (3 & 0xFF));  addr += 2; // LOADI R1, 3
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();

        std::cout << "5 - 3: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expecting 2) \n";
        std::cout << (cpu.getRegister(2) == 2 ? "PASS" : "FAIL") << "\n";
    }

    // Test 2: a = 3, b = 3. Intended results: result = 0, Z should be set
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (3 & 0xFF));  addr += 2; // LOADI R0, 3
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (3 & 0xFF));  addr += 2; // LOADI R1, 3
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();

        std::cout << "3 - 3: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expecting 0) \n";
        std::cout << (cpu.getRegister(2) == 0 ? "PASS" : "FAIL") << "\n";

        std::cout << "  Z flag = " << cpu.getZeroFlag() << " (expecting 1) \n";
        std::cout << (cpu.getZeroFlag() == true ? "PASS" : "FAIL") << "\n";
    }

    // Test 3: a = 3, b = 5. Intended results: result = 254 (meaning -2 wrapped), N should be set
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (3 & 0xFF));  addr += 2; // LOADI R0, 3
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (5 & 0xFF));  addr += 2; // LOADI R1, 5
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();

        std::cout << "3 - 5: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expecting 254) \n";
        std::cout << (cpu.getRegister(2) == 254 ? "PASS" : "FAIL") << "\n";

        std::cout << "  N flag = " << cpu.getNegativeFlag() << " (expecting 1) \n";
        std::cout << (cpu.getNegativeFlag() == true ? "PASS" : "FAIL") << "\n";
    }

    // LOAD/STORE test: 2 tests, One with positive offset and one with negative offset and overflow
    std::cout << "LOAD/STORE test\n";

    // Test 1: STORE R0's value at address (R1 + offset), then LOAD it back into R2.
    {
        cpu.reset();
        uint16_t addr = 0x0000;

        // R0 = 0x42 (value to store)
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (0x42 & 0xFF));  addr += 2;

        // R1 = 10 (base address for indirect addressing)
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (10 & 0xFF));    addr += 2;

        uint8_t offsetBits = static_cast<uint8_t>(5) & 0x1F;

        memory.writeWord(addr, (0b10001 << 11) | (0 << 8) | (1 << 5) | offsetBits); addr += 2; // STORE R0, [R1 + 5]. Sets memory[15] = 0x42
        memory.writeWord(addr, (0b10000 << 11) | (2 << 8) | (1 << 5) | offsetBits); addr += 2; // LOAD R2, [R1 + 5]. Sets R2 = memory[15]
        memory.writeWord(addr, (OP_HALT << 11));

        cpu.run();
        std::cout << "STORE 0x42 at [R1+5], LOAD into R2: R2 = 0x" << std::hex << static_cast<int>(cpu.getRegister(2)) << " (expecting 0x42) \n";
        std::cout  << (cpu.getRegister(2) == 0x42 ? "PASS" : "FAIL") << std::dec << "\n";
    }

    // Test 2: Negative offset test: STORE at [R1 - 3], LOAD back
    {
        cpu.reset();
        uint16_t addr = 0x0000;

        // R0 = 0x7A
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (0x7A & 0xFF));  addr += 2;
        // R1 = 20
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (20 & 0xFF));    addr += 2;

        uint8_t negOffsetBits = static_cast<uint8_t>(-3) & 0x1F; // offset = -3. 5-bit two's complement is 0b11101 (29)

        memory.writeWord(addr, (0b10001 << 11) | (0 << 8) | (1 << 5) | negOffsetBits); addr += 2; // STORE R0, [R1-3] -> memory[17]
        memory.writeWord(addr, (0b10000 << 11) | (2 << 8) | (1 << 5) | negOffsetBits); addr += 2; // LOAD R2, [R1-3]
        memory.writeWord(addr, (OP_HALT << 11));

        cpu.run();
        std::cout << "STORE 0x7A at [R1-3], LOAD into R2: R2 = 0x" << std::hex << static_cast<int>(cpu.getRegister(2)) << " (expecting 0x7a) \n";
        std::cout << (cpu.getRegister(2) == 0x7A ? "PASS" : "FAIL") << std::dec << "\n";
    }

    // JLT/JGE test: 4 tests, each confirming the functionality of JLT (Jump if less than) and JGE (Jump if greater than or equal to)
    std::cout << "JLT/JGE test\n";

    const uint8_t OP_JLT = 0b10101;
    const uint8_t OP_JGE = 0b10110;

    // Test 1: JLT: a = 3, b = 5. Since 3 < 5 is true, it should jump. Should skip the LOADI instruction that sets R3 = 99
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (3 & 0xFF));  addr += 2; // LOADI R0, 3
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (5 & 0xFF));  addr += 2; // LOADI R1, 5
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_JLT   << 11) | (1 & 0xFF)); addr += 2; // JLT +1 (skip next instruction)
        memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (99 & 0xFF)); addr += 2; // LOADI R3, 99 (should be skipped)
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "JLT taken (3 < 5 is true): R3 = " << static_cast<int>(cpu.getRegister(3)) << " (expecting 0, meaning jump was taken) \n";
        std::cout << (cpu.getRegister(3) == 0 ? "PASS" : "FAIL") << "\n";
    }

    // Test 2: JLT: a = 5, b = 3. Since 5 < 3 is false, it should NOT jump. R3 should get set to 99
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (5 & 0xFF));  addr += 2; // LOADI R0, 5
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (3 & 0xFF));  addr += 2; // LOADI R1, 3
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_JLT   << 11) | (1 & 0xFF)); addr += 2; // JLT +1
        memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (99 & 0xFF)); addr += 2; // LOADI R3, 99
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "JLT not taken (5 < 3 is false): R3 = " << static_cast<int>(cpu.getRegister(3)) << " (expecting 99, meaning jump was NOT taken) \n";
        std::cout << (cpu.getRegister(3) == 99 ? "PASS" : "FAIL") << "\n";
    }

    // Test 3: JGE: a = 5, b = 3. Since 5 >= 3 is true, it should jump. Should skip the LOADI instruction that sets R3 = 99
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (5 & 0xFF));  addr += 2; // LOADI R0, 5
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (3 & 0xFF));  addr += 2; // LOADI R1, 3
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_JGE   << 11) | (1 & 0xFF)); addr += 2; // JGE +1
        memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (99 & 0xFF)); addr += 2; // LOADI R3, 99
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "JGE taken (5 >=3 is true): R3 = " << static_cast<int>(cpu.getRegister(3)) << " (expecting 0, meaning jump was taken) \n";
        std::cout << (cpu.getRegister(3) == 0 ? "PASS" : "FAIL") << "\n";
    }

    // Test 4: JGE: a = 3, b = 5. Since 3 >= 5 is false, it should NOT jump. R3 should get set to 99
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (3 & 0xFF));  addr += 2; // LOADI R0, 3
        memory.writeWord(addr, (OP_LOADI << 11) | (1 << 8) | (5 & 0xFF));  addr += 2; // LOADI R1, 5
        memory.writeWord(addr, (OP_SUB   << 11) | (2 << 8) | (0 << 5) | (1 << 2)); addr += 2; // SUB R2, R0, R1
        memory.writeWord(addr, (OP_JGE   << 11) | (1 & 0xFF)); addr += 2; // JGE +1
        memory.writeWord(addr, (OP_LOADI << 11) | (3 << 8) | (99 & 0xFF)); addr += 2; // LOADI R3, 99
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "JGE not taken (3 >=5 is false): R3 = " << static_cast<int>(cpu.getRegister(3)) << " (expecting 99, meaning jump was NOT taken) \n";
        std::cout << (cpu.getRegister(3) == 99 ? "PASS" : "FAIL") << "\n";
    }

    // ADDI test
    std::cout << "ADDI test\n";

    const uint8_t OP_ADDI = 0b01001;

    // Test 1: R0 = 10, ADDI R0, 5. Sets R0 = 15
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (10 & 0xFF)); addr += 2; // LOADI R0, 10
        memory.writeWord(addr, (OP_ADDI  << 11) | (0 << 8) | (5 & 0xFF));  addr += 2; // ADDI R0, 5
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "10 + 5: R0 = " << static_cast<int>(cpu.getRegister(0)) << " (expecting 15) \n";
        std::cout << (cpu.getRegister(0) == 15 ? "PASS" : "FAIL") << "\n";
    }

    // Test 2: R0 = 0, ADDI R0, 0. Sets R0 = 0. Z should be set
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (0 & 0xFF));  addr += 2; // LOADI R0, 0
        memory.writeWord(addr, (OP_ADDI  << 11) | (0 << 8) | (0 & 0xFF));  addr += 2; // ADDI R0, 0
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "0 + 0: R0 = " << static_cast<int>(cpu.getRegister(0)) << " (expecting 0) \n";
        std::cout << (cpu.getRegister(0) == 0 ? "PASS" : "FAIL") << "\n";

        std::cout << "  Z flag = " << cpu.getZeroFlag() << " (expecting 1) \n";
        std::cout << (cpu.getZeroFlag() == true ? "PASS" : "FAIL") << "\n";
    }

    // Test 3: R0 = 200, ADDI R0, 100. Should wrap past 255. Carry should be set
    {
        cpu.reset();
        uint16_t addr = 0x0000;
        memory.writeWord(addr, (OP_LOADI << 11) | (0 << 8) | (200 & 0xFF)); addr += 2; // LOADI R0, 200
        memory.writeWord(addr, (OP_ADDI  << 11) | (0 << 8) | (100 & 0xFF)); addr += 2; // ADDI R0, 100
        memory.writeWord(addr, (OP_HALT  << 11));

        cpu.run();
        std::cout << "200 + 100: R0 = " << static_cast<int>(cpu.getRegister(0)) << " (expecting 44, i.e. 300 mod 256) \n";
        std::cout << (cpu.getRegister(0) == 44 ? "PASS" : "FAIL") << "\n";

        std::cout << "  C flag = " << cpu.getCarryFlag() << " (expecting 1) \n";
        std::cout << (cpu.getCarryFlag() == true ? "PASS" : "FAIL") << "\n";
    }
}

// FULL PROGRAM TESTS

void runFactorialTest(CPU& cpu, Memory& memory) {
    loadFactorialProgram(memory);
    cpu.reset(0x0100);
    cpu.run();

    int expected[6] = {1, 1, 2, 6, 24, 120};
    bool allPassed = true;
    for (int i = 0; i < 6; i++) {
        uint8_t val = memory.readByte(static_cast<uint16_t>(i));
        std::cout << i << "! = " << static_cast<int>(val) << " (expect " << expected[i] << ")\n";
        std::cout << (val == expected[i] ? "PASS" : "FAIL") << "\n";

        if (val != expected[i]) {
            allPassed = false;
        }
    }
    std::cout << (allPassed ? "Factorial table: all passed\n" : "Factorial table: check failures above\n");
}

void runFibonacciTest(CPU& cpu, Memory& memory) {
    loadFibonacciProgram(memory);
    cpu.reset(0x0100);
    cpu.run();

    int expectedFib[14] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};
    bool allFibPassed = true;
    for (int i = 0; i < 14; i++) {
        uint8_t val = memory.readByte(static_cast<uint16_t>(i));
        std::cout << "fib(" << i << ") = " << static_cast<int>(val) << " (expect " << expectedFib[i] << ")\n";
        std::cout << (val == expectedFib[i] ? "PASS" : "FAIL") << "\n";

        if (val != expectedFib[i]) {
            allFibPassed = false;
        }
    }
    std::cout << (allFibPassed ? "Fibonacci table: all passed\n" : "Fibonacci table: check failures above\n");
}

void runBubbleSortTest(CPU& cpu, Memory& memory) {
    loadBubbleSortProgram(memory);
    cpu.reset(0x0100);
    cpu.run();

    int expectedSorted[5] = {1, 3, 5, 7, 15};
    bool allSortPassed = true;
    for (int i = 0; i < 5; i++) {
        uint8_t val = memory.readByte(static_cast<uint16_t>(i));
        std::cout << "arr[" << i << "] = " << static_cast<int>(val) << " (expect " << expectedSorted[i] << ")\n";
        std::cout << (val == expectedSorted[i] ? "PASS" : "FAIL") << "\n";

        if (val != expectedSorted[i]) {
            allSortPassed = false;
        }
    }
    std::cout << (allSortPassed ? "Bubble sort: all passed\n" : "Bubble sort: check failures above\n");
}

int main() {
    Memory memory;
    CPU cpu(memory);

    std::cout << "8-bit CPU emulator skeleton initialised.\n";
    std::cout << "Memory size: " << Memory::memorySize << " bytes\n";
    std::cout << "Register count: " << CPU::registerNum << "\n";
    std::cout << "\n";

    // Can comment out undesired tests

    // runInstructionTests(cpu, memory);
    // runFactorialTest(cpu, memory);
    // runFibonacciTest(cpu, memory);
    runBubbleSortTest(cpu, memory);

    return 0;
}