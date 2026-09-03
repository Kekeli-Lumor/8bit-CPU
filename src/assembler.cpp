#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "assembler.hpp"

const std::map<std::string, InstructionInfo> instructionTable = {
    {"ADD",   {0b00000, Format::R_TYPE}},
    {"SUB",   {0b00001, Format::R_TYPE}},
    {"HALT",  {0b00010, Format::R_TYPE_NO_OPERANDS}},
    {"MOV",   {0b00011, Format::R_TYPE_TWO_OPERANDS}},
    {"LOADI", {0b01000, Format::I_TYPE}},
    {"ADDI",  {0b01001, Format::I_TYPE}},
    {"LOAD",  {0b10000, Format::INDIRECT_TYPE}},
    {"STORE", {0b10001, Format::INDIRECT_TYPE}},
    {"JMP",   {0b10010, Format::JUMP_TYPE}},
    {"JEQ",   {0b10011, Format::JUMP_TYPE}},
    {"JNE",   {0b10100, Format::JUMP_TYPE}},
    {"JLT",   {0b10101, Format::JUMP_TYPE}},
    {"JGE",   {0b10110, Format::JUMP_TYPE}},
};


// Removes trailing "//" comment (if present) and leading/trailing whitespace from a line
// Used by parseLine
static std::string removeCommentAndTrim(const std::string& line) {
    std::string result = line;
    size_t commentPos = result.find("//");

    if (commentPos != std::string::npos) {
        result = result.substr(0, commentPos);
    }

    size_t start = result.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return ""; // Line was all whitespace or empty after stripping
    }

    size_t end = result.find_last_not_of(" \t");
    return result.substr(start, end - start + 1);
}


// Splits comma-separated operand string (e.g. "R1, R2, R3") into individual operands ("R1", "R2", "R3")
static std::vector<std::string> splitOperands(const std::string& operandString) {
    std::vector<std::string> operands;
    std::stringstream ss(operandString);
    std::string operand;

    while (std::getline(ss, operand, ',')) {
        size_t start = operand.find_first_not_of(" \t");

        if (start != std::string::npos){ // Skips entries that are either empty or white space
            size_t end = operand.find_last_not_of(" \t");
            operands.push_back(operand.substr(start, end - start + 1));
        }
    }

    return operands;
}


// Converts a string operand into its numeric value. Examples:
// - "R3" becomes 3, still representing register 3
// - "5" or "-10" become plain immediates 5 or -10
static int operandToInt(const std::string& operand) {
    if (!operand.empty() && (operand[0] == 'R' || operand[0] == 'r')) {
        return std::stoi(operand.substr(1));
    }
    return std::stoi(operand);
}


ParsedLine parseLine(const std::string& assemblyLine) {
    ParsedLine result;

    std::string line = removeCommentAndTrim(assemblyLine);
    if (line.empty()) {
        return result;
    }

    // Separate label from instruction (e.g. "LoopStart: ADD R1, R2, R3")
    std::string instructionPart = line;
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        result.labelPresent = true;
        result.label = removeCommentAndTrim(line.substr(0, colonPos));
        instructionPart = removeCommentAndTrim(line.substr(colonPos + 1));
    }

    if (instructionPart.empty()) {
        return result; // label-only line
    }

    // Separate mnemonic from everything after it
    size_t spacePos = instructionPart.find(' ');
    std::string mnemonic;
    std::string operandString;

    if (spacePos == std::string::npos) {
        mnemonic = instructionPart;
        operandString = "";
    } else {
        mnemonic = instructionPart.substr(0, spacePos);
        operandString = instructionPart.substr(spacePos + 1);
    }

    std::vector<std::string> operands = splitOperands(operandString);

    const InstructionInfo& info = instructionTable.at(mnemonic);

    ParsedInstruction instr;
    instr.mnemonic = mnemonic;

    switch (info.format) {
        case Format::R_TYPE:
            instr.operand1 = operandToInt(operands.at(0));
            instr.operand2 = operandToInt(operands.at(1));
            instr.operand3 = operandToInt(operands.at(2));
            break;

        case Format::R_TYPE_TWO_OPERANDS:
            instr.operand1 = operandToInt(operands.at(0));
            instr.operand2 = operandToInt(operands.at(1));
            break;

        case Format::R_TYPE_NO_OPERANDS:
            break;

        case Format::I_TYPE:
            instr.operand1 = operandToInt(operands.at(0));
            instr.immediate = operandToInt(operands.at(1));
            break;

        case Format::INDIRECT_TYPE:
            instr.operand1 = operandToInt(operands.at(0));
            instr.operand2 = operandToInt(operands.at(1));
            instr.immediate = operandToInt(operands.at(2));
            break;

        case Format::JUMP_TYPE:
            instr.jumpTarget = operands.at(0);
            break;
    }

    result.instructionPresent = true;
    result.instruction = instr;
    return result;
}



uint16_t encodeInstruction(const ParsedInstruction& instr) {
    const InstructionInfo& info = instructionTable.at(instr.mnemonic);
    uint16_t opcodeField = static_cast<uint16_t>(info.opcode) << 11;

    switch (info.format) {
        case Format::R_TYPE:
            return opcodeField | (static_cast<uint16_t>(instr.operand1) << 8) | (static_cast<uint16_t>(instr.operand2) << 5) | (static_cast<uint16_t>(instr.operand3) << 2);

        case Format::R_TYPE_TWO_OPERANDS:
            return opcodeField | (static_cast<uint16_t>(instr.operand1) << 8) | (static_cast<uint16_t>(instr.operand2) << 5);

        case Format::R_TYPE_NO_OPERANDS:
            return opcodeField;

        case Format::I_TYPE:
            return opcodeField | (static_cast<uint16_t>(instr.operand1) << 8) | (static_cast<uint16_t>(instr.immediate) & 0xFF);

        case Format::INDIRECT_TYPE:
            return opcodeField | (static_cast<uint16_t>(instr.operand1) << 8) | (static_cast<uint16_t>(instr.operand2) << 5) | (static_cast<uint16_t>(instr.immediate) & 0x1F);

        case Format::JUMP_TYPE:
            return opcodeField | (static_cast<uint16_t>(instr.immediate) & 0xFF);
    }

    return 0; // Should be unreachable if all Format cases are correctly handles
}


std::vector<uint16_t> assembleProgram(const std::vector<std::string>& lines) {
    // First Pass to create symbol table
    std::map<std::string, uint16_t> symbolTable;
    uint16_t currentAddress = 0x0100;

    for (const std::string& line : lines) {
        ParsedLine parsed = parseLine(line);
        if (parsed.labelPresent) {
            symbolTable[parsed.label] = currentAddress;
        }
        if (parsed.instructionPresent) {
            currentAddress += 2;
        }
    }

    // Second pass to encode all instructions
    std::vector<uint16_t> machineCode;
    currentAddress = 0x0100;

    for (const std::string& line : lines) {
        ParsedLine parsed = parseLine(line);

        if (parsed.instructionPresent) {
            ParsedInstruction instr = parsed.instruction;
            const InstructionInfo& info = instructionTable.at(instr.mnemonic);

            if (info.format == Format::JUMP_TYPE) {
                uint16_t targetAddress = symbolTable.at(instr.jumpTarget);

                // PC-relative offset in instruction-units, measured from the address after this instruction since PC has already advanced by the time it runs
                int byteOffset = static_cast<int>(targetAddress) - static_cast<int>(currentAddress + 2);
                instr.immediate = byteOffset / 2;
            }

            machineCode.push_back(encodeInstruction(instr));
            currentAddress += 2;
        }
    }

    return machineCode;
}