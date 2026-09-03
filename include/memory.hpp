#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

// Represents CPU's addressable memory: 64KB, byte-addressable.
// Separate component from CPU like real hardware, where the CPU and memory are distinct units connected by an address/data bus
class Memory {
public:
    static constexpr size_t memorySize = 65536; // 64KB
    

    Memory();

    // Read a single byte at the given 16-bit address
    uint8_t readByte(uint16_t address) const;

    // Write a single byte at the given 16-bit address
    void writeByte(uint16_t address, uint8_t value);

    // Read a 16-bit instruction word starting at the given address
    // Little-endian: low byte at address, high byte at address+1
    // Used for instruction fetch only — data access
    // is always via readByte/writeByte, since General Purpose Registers are 8-bit
    uint16_t readWord(uint16_t address) const;

    //Write a 16-bit word starting at the given address, little-endian
    void writeWord(uint16_t address, uint16_t value);

private:
    std::array<uint8_t, memorySize> memoryData;
    
};