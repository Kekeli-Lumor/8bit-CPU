#include <iostream>

#include "cpu.hpp"
#include "memory.hpp"
#include "cstdint"

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
    std::cout << "Bytes written: 0x34 and 0x12. Value after readWord = 0x" << std::hex << wordResult << " (expect 0x1234)" << "\n";
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
    const uint8_t OP_ADD   = 0b00000;
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

        std::cout << "5 - 3: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expect 2) \n";
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

        std::cout << "3 - 3: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expect 0) \n";
        std::cout << (cpu.getRegister(2) == 0 ? "PASS" : "FAIL") << "\n";

        std::cout << "  Z flag = " << cpu.getZeroFlag() << " (expect 1) \n";
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

        std::cout << "3 - 5: R2 = " << static_cast<int>(cpu.getRegister(2)) << " (expect 254) \n";
        std::cout << (cpu.getRegister(2) == 254 ? "PASS" : "FAIL") << "\n";

        std::cout << "  N flag = " << cpu.getNegativeFlag() << " (expect 1) \n";
        std::cout << (cpu.getNegativeFlag() == true ? "PASS" : "FAIL") << "\n";
    }

    return 0;
}