#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "memory.hpp"

// Handles File Input and Output for assembly programs
// Reads .asm files, writes machine code to .bin files and loads them into Memory


// Reads .asm file and return each line as a string stored in a vector
std::vector<std::string> readAsmFile(const std::string& asmFile);


// Writes machine code generated from assembly file to raw binary file
void writeBinFile(const std::string& binFile, const std::vector<uint16_t>& machineCode);


// Reads .bin file and loads contents into memory starting from 0x0100
void loadBinFile(const std::string& binFile, Memory& memory);
