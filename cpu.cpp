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
    // TODO (Week 3): read 2 bytes from memory at programCounter, combine
    // into a 16-bit instruction, advance programCounter by 2.
    // Placeholder: halts immediately so run() doesn't loop forever.
    halted = true;
    return 0;
}

void CPU::decodeAndExecute(uint16_t instruction) {
    // TODO (Week 3): decode opcode, dispatch to the correct instruction
    // handler per ISA.md Section 5's opcode table.
    (void)instruction; // suppress unused-parameter warning until implemented
}