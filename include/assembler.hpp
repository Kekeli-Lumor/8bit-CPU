#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

// Metadata and lookup table for translating assembly mnemonics into machine code

// Identifies which instruction format a mnemonic uses, so the parser knows how many operands to expect and 
// the encoder knows how to pack the bits
enum class Format {
    R_TYPE, // e.g. ADD Rd, Rs1, Rs2
    R_TYPE_NO_OPERANDS, // e.g. HALT
    R_TYPE_TWO_OPERANDS, // e.g. MOV Rd, Rs1
    I_TYPE, // e.g. LOADI Rd, imm
    INDIRECT_TYPE, // e.g. Rd, Rs, offset
    JUMP_TYPE, // e.g. JMP LabelName
};

struct InstructionInfo {
    uint8_t opcode;
    Format format;
};

// A decoded line of assembly ready to be encoded into machine code
// Not every field is used by every format. Encoder consults instruction's Format to know which fields are relevant
struct ParsedInstruction {
    std::string mnemonic;
    int operand1 = 0; // destination register or unused
    int operand2 = 0; // source1 register or base register (indirect) or unused
    int operand3 = 0; // source2 register or unused
    int immediate = 0; // immediate offset (indirect/jump)
    std::string jumpTarget; // stores label name for when a jump is called
};


// Result of parsing one assembly line. Boolean values let caller know what a line actually contains
struct ParsedLine{
    bool labelPresent = false;
    std::string label;

    bool instructionPresent = false;
    ParsedInstruction instruction;
};

// Parses single assembly line into a ParsedLine, removing white spaces, comments etc
ParsedLine parseLine(const std::string& assemblyLine);

// Encodes parsed instruction into 16-bit machine code
uint16_t encodeInstruction(const ParsedInstruction& instr);

// Assembles full program in 2 passes: 
// - First pass to create a symbol table of all the addresses referred to by Labels
// - Second pass to encode all instructions and create jump targets from the symbol table
std::vector<uint16_t> assembleProgram(const std::vector<std::string>& lines);