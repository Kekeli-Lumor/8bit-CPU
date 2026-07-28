#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

//Represents CPU's addressable memory: 64KB, byte-addressable.
//Separate component from CPU (not owned by it) — mirrors real hardware, where the CPU and memory are distinct units connected by
//an address/data bus.
//NOTE: read/write logic is not yet implemented (Week 2 — Memory Model).
class Memory {
public:
    static constexpr size_t memorySize = 65536; // 64KB

    Memory();

    //Read a single byte at the given 16-bit address.
    uint8_t readByte(uint16_t address) const;

    //Write a single byte at the given 16-bit address.
    void writeByte(uint16_t address, uint8_t value);

private:
std::array<uint8_t, memorySize> memoryData;
};