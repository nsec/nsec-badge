#ifndef CHALLENGE4_VM_H
#define CHALLENGE4_VM_H

enum vm_status {
    STATUS_RESET = 0,
    STATUS_IS_RUNNING
};

enum vm_register {
    REGISTER_NONE = 0,
    REGISTER_A = 1, // accumulator
    REGISTER_B = 2, // base register
};

struct vm {
    // VM State
    enum vm_status status;

    // Buffers
    uint16_t program[256];
    uint8_t memory[256];
    uint8_t stack[256];

    // Registers
    uint8_t pc; // program counter
    uint8_t sp; // stack pointer
    uint8_t a; // a register
    uint8_t b; // b register

    // Flags
    uint8_t zf; // zero flag
};

void vm_reset(struct vm *vm);
void vm_run(struct vm *vm);
void vm_exec(struct vm *vm, uint16_t opcode);

#endif
