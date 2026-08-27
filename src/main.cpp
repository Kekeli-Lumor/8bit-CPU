#include <iostream>
#include <cstdint>

#include "cpu.hpp"
#include "memory.hpp"


int main() {
    Memory memory;
    CPU cpu(memory);

    std::cout << "8-bit CPU emulator skeleton initialised.\n";
    std::cout << "Memory size: " << Memory::memorySize << " bytes\n";
    std::cout << "Register count: " << CPU::registerNum << "\n";
    std::cout << "\n";

  

    /*
    
    // Byte read and write test
    memory.writeByte(0x0010, 0xAB);
    uint8_t byteResult = memory.readByte(0x0010);
    std::cout << "Byte test: wrote 0x" << std::hex << static_cast<int>(byteResult) << "\n";
    std::cout << (byteResult == 0xAB ? "PASS" : "FAIL") << "\n";


    // Write word test. Confirm byte order is little endian
    memory.writeWord(0x0020, 0xCDAB);
    uint8_t lowByteResult = memory.readByte(0x0020);
    uint8_t highByteResult = memory.readByte(0x0021);
    std::cout << "Word write: wrote 0xCDAB. Value at 0x0020: 0x" << std::hex << static_cast<int>(lowByteResult) << " expecting 0x0ab. Value at 0x0021: 0x"
    << std::hex << static_cast<int>(highByteResult) << " expecting 0xcd \n";
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
    cpu.reset(); // set PC to 0x0100 if reset() doesn't already do this
    cpu.run();
    std::cout << (cpu.isHalted() ? "CPU halted correctly" : "CPU did NOT halt") << "\n";

    

    // LOAD and move test
    memory.writeWord(0x0000, 0b0100000000000001); // LOADI R0, imm=1
    memory.writeWord(0x0002, 0b0001100000000000); // MOV R1, R0
    memory.writeWord(0x0004, 0b0001000000000000); // HALT

    cpu.run();

    std::cout << "R0 = " << (int)cpu.getRegister(0) << "\n";

    */ 

    


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
    // Reuses some instruction building fields from earlier so make sure relevent variables (under SUB test) aren't commented out

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
    // Reuses some instruction building fields from earlier so make sure relevent variables (under SUB test) aren't commented out
    
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

    // Test 3: JGE: a = 5, b = 3. Since 5 >= 3 is true, it should jump. Should skip skip the LOADI instruction that sets R3 = 99
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



    return 0;
}