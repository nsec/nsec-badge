#pragma once

enum instruction {
    INSTR_RESET = 0,
    // Outside world
    INSTR_IN,           // Read a byte from input port to register A/B
    INSTR_OUT,          // Write a byte from register A/B to output port
    // Arythmetics
    INSTR_ADD,          // Add immediate to register A/B
    INSTR_ADDM,         // Add from address to register A/B
    INSTR_SUB,          // Add immediate to register A/B
    INSTR_XOR,          // Xor A with immediate, set ZF
    // Memory
    INSTR_LOAD_IMM,     // Load immediate into register A/B
    INSTR_LOAD,         // Load address (B) into register A
    INSTR_STORE,        // Store A/B register into address
    // Stack operation
    INSTR_PUSH,         // Push A/B/immediate on the stack
    INSTR_POP,          // Pop from the stack into register A/B
    // Conditionnals
    INSTR_JMP,          // Jump to address
    INSTR_JMPZ,         // Jump to address, if non-zero
};

