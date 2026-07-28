#include <iostream>

#include "cpu.hpp"
#include "memory.hpp"

int main() {
    Memory memory;
    CPU cpu(memory);

    std::cout << "8-bit CPU emulator skeleton initialised.\n";
    std::cout << "Memory size: " << Memory::memorySize << " bytes\n";
    std::cout << "Register count: " << CPU::registerNum << "\n";

    // No program loaded yet — run() would halt immediately via the
    // placeholder fetchInstruction(). Left uncalled here until Week 3.

    return 0;
}