#include <fstream>
#include <stdexcept>

#include "programloader.hpp"

// Reads asm file and throws appropriate error if it can't be opened
std::vector<std::string> readAsmFile(const std::string& asmFile){
    std::vector<std::string> lines;
    std::ifstream file(asmFile);

    if (!file.is_open()){
        throw std::runtime_error("Could not open file: " + asmFile);
    }

    std::string line;
    while (std::getline(file, line)){
        lines.push_back(line);
    }

    return lines;

}

// Writes bin file and throws appropriate error if it can't be opened
void writeBinFile(const std::string& binFile, const std::vector<uint16_t>& machineCode){
    std::ofstream file(binFile, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + binFile);
    }

    for (uint16_t instruction : machineCode) {
        file.write(reinterpret_cast<const char*>(&instruction), sizeof(instruction));
    }

}

// Reads bin file and throws appropriate error if it can't be opened
void loadBinFile(const std::string& binFile, Memory& memory){
    std::ifstream file(binFile, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + binFile);
    }

    uint16_t address = 0x0100;
    uint16_t instruction;

    while (file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        memory.writeWord(address, instruction);
        address += 2;
    }

}