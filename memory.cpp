#include "memory.hpp"

Memory::Memory() {
    memoryData.fill(0);
}

uint8_t Memory::readByte(uint16_t address) const {
    // TODO (Week 2): implement actual read logic.
    return memoryData[address];
}

void Memory::writeByte(uint16_t address, uint8_t value) {
    // TODO (Week 2): implement actual write logic.
    memoryData[address] = value;
}