#include <cassert>

#include "cpu.hpp"


CPU::CPU(Memory& memoryRef) : memory(memoryRef), programCounter(0), flags(0), halted(false) {
    registers.fill(0);
}

void CPU::reset() {
    registers.fill(0);
    programCounter = 0;
    flags = 0;
    halted = false;
}

void CPU::step() {
    // TODO (Week 3): fetch, decode, execute one instruction.
    uint16_t instruction = fetchInstruction();
    decodeAndExecute(instruction);
}

void CPU::run() {
    // TODO (Week 3): loop step() until halted becomes true.
    while (!halted) {
        step();
    }
}

bool CPU::isHalted() const {
    return halted;
}

uint16_t CPU::fetchInstruction() {
    uint16_t instruction = memory.readWord(programCounter);
    programCounter += 2;
    return instruction;
}

uint8_t CPU::getRegister(uint8_t index) const {
    assert(index < registerNum);
    return registers[index];
}

bool CPU::getZeroFlag() const {
    return (flags >> 3) & 0x1;
}

bool CPU::getNegativeFlag() const {
    return (flags >> 2) & 0x1;
}

bool CPU::getCarryFlag() const {
    return (flags >> 1) & 0x1;
}

bool CPU::getOverflowFlag() const {
    return flags & 0x1;
}





void CPU::decodeAndExecute(uint16_t instruction) {
    uint8_t opcode = (instruction >> 11) & 0x1F;  // Shifts opcode bits (top 5 bits) to bottom of value and keeps only those 5 bits using & 0x1F (bitwise operation)
    uint8_t dest = (instruction >> 8) & 0x7;

    if (opcode == 0b00010){
        // HALT instruction
        halted = true; 

    } else if (opcode == 0b00011){
        // MOV Rd, Rs1. Sets Rd = Rs1
        uint8_t src1 = (instruction >> 5) & 0x7;
        registers[dest] = registers[src1];

    } else if (opcode == 0b01000){
        // LOADI Rd, imm. Sets Rd = immediate
        uint8_t immediate = instruction & 0xFF;
        registers[dest] = immediate;

    } else if (opcode == 0b00000) {
        // ADD Rd, Rs1, Rs2. Sets Rd = Rs1 + Rs2 and updates flags
        uint8_t src1 = (instruction >> 5) & 0x7;
        uint8_t src2 = (instruction >> 2) & 0x7;

        uint8_t a = registers[src1];
        uint8_t b = registers[src2];
        uint16_t widenedResult = static_cast<uint16_t>(a) + static_cast<uint16_t>(b); // Done so carry out is visible before truncation for actual result in the next line
        uint8_t result = static_cast<uint8_t>(widenedResult);

        registers[dest] = result; 

        // Z: result is zero
        bool zFlag = (result == 0);

        // N: bit 7 (sign bit) of result is 1
        bool nFlag = (result & 0x80) != 0;

        // C: unsigned addition overflowed 8 bits
        bool cFlag = (widenedResult > 255);

        // V: operands share a sign, result's sign differs
        bool signA = (a & 0x80) != 0;
        bool signB = (b & 0x80) != 0;
        bool signResult = (result & 0x80) != 0;
        bool vFlag = (signA == signB) && (signResult != signA);

        flags = (zFlag << 3) | (nFlag << 2) | (cFlag << 1) | vFlag;

    } else if (opcode == 0b00001) {
        // SUB Rd, Rs1, Rs2. Sets Rd = Rs1 - Rs2 and updates flags
        uint8_t src1 = (instruction >> 5) & 0x7;
        uint8_t src2 = (instruction >> 2) & 0x7;

        uint8_t a = registers[src1];
        uint8_t b = registers[src2];
        uint8_t negatedB = static_cast<uint8_t>(~b + 1); // two's complement negation of Rs2
        uint16_t widenedResult = static_cast<uint16_t>(a) + static_cast<uint16_t>(negatedB);
        uint8_t result = static_cast<uint8_t>(widenedResult);

        registers[dest] = result;

        // Z: result is zero
        bool zFlag = (result == 0);

        // N: bit 7 (sign bit) of result is 1
        bool nFlag = (result & 0x80) != 0;

        // C: unsigned addition overflowed 8 bits
        bool cFlag = (widenedResult > 255);

        // V: operands share a sign, result's sign differs
        bool signA = (a & 0x80) != 0;
        bool signB = (negatedB & 0x80) != 0;
        bool signResult = (result & 0x80) != 0;
        bool vFlag = (signA == signB) && (signResult != signA);

        flags = (zFlag << 3) | (nFlag << 2) | (cFlag << 1) | vFlag;
    }

    
    
    else{
        // Other instructions not implemented yes. Halted for now
        halted = true;
    }
}