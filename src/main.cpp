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

    // placeholder fetchInstruction(). Left uncalled here until Week 3.

    //Test 1: byte read and write
    memory.writeByte(0x0010, 0xAB);
    uint8_t byteResult = memory.readByte(0x0010);
    std::cout << "Byte test: wrote 0x" << std::hex << static_cast<int>(byteResult) << "\n";
    std::cout << (byteResult == 0xAB ? "PASS" : "FAIL") << "\n";

    //Test 2: write word. COnfirm byte order is little endian
    memory.writeWord(0x0020, 0xCDAB);
    uint8_t lowByteResult = memory.readByte(0x0020);
    uint8_t highByteResult = memory.readByte(0x0021);
    std::cout << "Word write: wrote 0xCDAB. Value at 0x0020: 0x" << std::hex << static_cast<int>(lowByteResult) << " expecting 0x0ab. Value at 0x0021: 0x"
    << std::hex << static_cast<int>(highByteResult) << " expecting 0xcd \n";
    std::cout <<  ((lowByteResult == 0xAB && highByteResult == 0xCD) ? "PASS" : "FAIL") << "\n";

    // Test 3: write two bytes separately. Confirm readWord reassembles them correctly
    memory.writeByte(0x0030, 0x34);
    memory.writeByte(0x0031, 0x12);
    uint16_t wordResult = memory.readWord(0x0030);
    std::cout << "Bytes written: 0x34 and 0x12. Value after readWord = 0x" << std::hex << wordResult << " (expect 0x1234)" << "\n";
    std::cout << (wordResult == 0x1234 ? "PASS" : "FAIL") << "\n";

    std::cout << std::dec;

    return 0;
}