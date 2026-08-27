#include "memory.hpp"
#include <cassert>

Memory::Memory() {
    memoryData.fill(0);
}

uint8_t Memory::readByte(uint16_t address) const {
    return memoryData[address];
}

void Memory::writeByte(uint16_t address, uint8_t value) {
    memoryData[address] = value;
}

uint16_t Memory::readWord(uint16_t address) const {
    assert(address != 0xFFFF && "readWord at 0xFFFF would read past end of memory");

    uint8_t lowByte = memoryData[address];
    uint8_t highByte = memoryData[address + 1];

    return (static_cast<uint16_t>(lowByte)) | (static_cast<uint16_t>(highByte) << 8);
}

void Memory::writeWord(uint16_t address, uint16_t value) {
    assert(address != 0xFFFF && "writeWord at 0xFFFF would write past end of memory");

    memoryData[address] = static_cast<uint8_t>(value & 0xFF);
    memoryData[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}