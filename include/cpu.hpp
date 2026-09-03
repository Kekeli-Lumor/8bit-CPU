#pragma once

#include <cstdint>
#include <array>

#include "memory.hpp"

// Represents the CPU's execution state and fetch-decode-execute cycle.
// Holds a reference to a Memory instance rather than owning one — CPU and
// Memory are distinct components (as in real hardware, connected by a bus),
// so ownership lives in main(), not here. See ISA.md Section 2 for the
// register file / PC / flags design this class implements.

class CPU {
public:
    static constexpr size_t registerNum = 8; // R0-R7

    explicit CPU(Memory& memoryRef);

    // Resets registers, PC, and flags to their initial state
    // startAddress was later added as an argument as it's required to read and write instructions to certain addresses rather than just from 0
    // default argument used so changed don't have to be made to testing at that point in time
    void reset(uint16_t startAddress = 0); 

    // Executes a single fetch-decode-execute cycle (one instruction)
    void step();

    // Runs step() repeatedly until a HALT instruction is executed
    void run();

    bool isHalted() const;

    // Getter for values in register R0-R7, for testing
    uint8_t getRegister(uint8_t index) const;

    // Getters for individual flag bits, for testing
    bool getZeroFlag() const;
    bool getNegativeFlag() const;
    bool getCarryFlag() const;
    bool getOverflowFlag() const;

private:
    Memory& memory;
    static constexpr int maxDataAddress = 270; // Register-indirect structural ceiling

    std::array<uint8_t, registerNum> registers; // R0-R7, 8-bit each
    uint16_t programCounter; // 16-bit
    uint8_t flags; // Bit layout: 3=Z 2=N 1=C 0=V 
    bool halted;

    uint16_t fetchInstruction();
    void decodeAndExecute(uint16_t instruction);
};