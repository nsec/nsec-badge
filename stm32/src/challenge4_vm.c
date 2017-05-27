//
// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)
//

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "instruction.h"
#include "challenge4_vm.h"

static uint8_t vm_do_invalid_instr(struct vm *vm, uint16_t opcode) {
    (void) opcode;

    printf("\nSIGILL pc: %02x, instr: %04x\n", vm->pc-1, vm->program[vm->pc-1]);
    vm->status = STATUS_RESET;
    return 0;
}

static uint8_t vm_do_reset(struct vm *vm, uint16_t opcode) {
    (void) opcode;

    printf("\nEXIT\n");
    vm->status = STATUS_RESET;
    return 0;
}

static uint8_t vm_do_in(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t oper1 = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) oper1;

    if (reg == REGISTER_A) {
        vm->a = getc(stdin);
    }
    else if (reg == REGISTER_B) {
        vm->b = getc(stdin);
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }

    return 0;
}

static uint8_t vm_do_out(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t oper1 = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) oper1;

    if (reg == REGISTER_A) {
        printf("%c", vm->a);
    }
    else if (reg == REGISTER_B) {
        printf("%c", vm->b);
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }

    return 0;
}

static void vm_do_load_imm(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg == REGISTER_A) {
        vm->a = imm;
    }
    else if (reg == REGISTER_B) {
        vm->b = imm;
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_load(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg & REGISTER_A && reg & REGISTER_B) {
        vm->a = vm->memory[vm->b];
    }
    else if (reg == REGISTER_A) {
        vm->a = vm->memory[imm];
    }
    else if (reg == REGISTER_B) {
        vm->b = vm->memory[imm];
    }
}

static void vm_do_store(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg & REGISTER_A && reg & REGISTER_B) {
        vm->memory[vm->b] = vm->a;
    }
    else if (reg == REGISTER_A) {
        vm->memory[imm] = vm->a;
    }
    else if (reg == REGISTER_B) {
        vm->memory[vm->b] = imm;
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_add(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg == REGISTER_A) {
        vm->a += imm;
    }
    else if (reg == REGISTER_B) {
        vm->b += imm;
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_addm(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg == REGISTER_A) {
        vm->a += vm->memory[imm];
    }
    else if (reg == REGISTER_B) {
        vm->b += vm->memory[imm];
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_xor(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg & REGISTER_A && reg & REGISTER_B) {
        vm->a = vm->a ^ vm->b;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_A) {
        vm->a = vm->a ^ imm;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_B) {
        vm->b = vm->b ^ imm;
        vm->zf = (vm->b == 0 ? 1 : 0);
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_sub(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;

    if (reg & REGISTER_A && reg & REGISTER_B) {
        vm->a = vm->a - vm->b;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_A) {
        vm->a = vm->a - imm;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_B) {
        vm->b = vm->b - imm;
        vm->zf = (vm->b == 0 ? 1 : 0);
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }
}

static void vm_do_jmp(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) reg;

    vm->pc = imm;
}

static void vm_do_jmpz(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) reg;

    if (vm->zf) {
        vm->pc = imm;
    }
}

static void vm_do_push(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) imm;

    if (reg == REGISTER_A) {
        vm->stack[vm->sp] = vm->a;
        vm->sp++;
    }
    else if (reg == REGISTER_B) {
        vm->stack[vm->sp] = vm->b;
        vm->sp++;
    }
}

static void vm_do_pop(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    (void) instr;
    (void) imm;

    if (reg == REGISTER_A) {
        vm->sp--;
        vm->a = vm->stack[vm->sp];
        vm->stack[vm->sp] = 0x00;
    }
    else if (reg == REGISTER_B) {
        vm->sp--;
        vm->b = vm->stack[vm->sp];
        vm->stack[vm->sp] = 0x00;
    }
}

static uint8_t vm_do_shift_left(struct vm *vm, uint16_t opcode) {
    uint8_t instr = (opcode & 0xf000) >> 12;
    uint8_t imm = (opcode & 0x0ff0) >> 4;
    uint8_t reg = (opcode & 0x000f);

    if (reg & REGISTER_A && reg & REGISTER_B) {
        vm->a = vm->a << vm->b;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_A) {
        vm->a = vm->a << imm;
        vm->zf = (vm->a == 0 ? 1 : 0);
    }
    else if (reg == REGISTER_B) {
        vm->b = vm->b << imm;
        vm->zf = (vm->b == 0 ? 1 : 0);
    }
    else {
        vm_do_invalid_instr(vm, opcode);
    }

    return 0;
}

void vm_exec(struct vm *vm, uint16_t opcode) {
    uint8_t instruction;

    instruction = (opcode & 0xf000) >> 0xc;

    switch (instruction) {
        case INSTR_RESET:
            vm_do_reset(vm, opcode);
            break;
        case INSTR_IN:
            vm_do_in(vm, opcode);
            break;
        case INSTR_OUT:
            vm_do_out(vm, opcode);
            break;
        case INSTR_LOAD_IMM:
            vm_do_load_imm(vm, opcode);
            break;
        case INSTR_LOAD:
            vm_do_load(vm, opcode);
            break;
        case INSTR_STORE:
            vm_do_store(vm, opcode);
            break;
        case INSTR_ADD:
            vm_do_add(vm, opcode);
            break;
        case INSTR_ADDM:
            vm_do_addm(vm, opcode);
            break;
        case INSTR_SUB:
            vm_do_sub(vm, opcode);
            break;
        case INSTR_XOR:
            vm_do_xor(vm, opcode);
            break;
        case INSTR_JMP:
            vm_do_jmp(vm, opcode);
            break;
        case INSTR_JMPZ:
            vm_do_jmpz(vm, opcode);
            break;
        case INSTR_PUSH:
            vm_do_push(vm, opcode);
            break;
        case INSTR_POP:
            vm_do_pop(vm, opcode);
            break;
        case INSTR_SHL:
            vm_do_shift_left(vm, opcode);
            break;
        default:
            vm_do_invalid_instr(vm, opcode);
            break;
    }
}

// Fetch the next opcode
static uint16_t vm_next_opcode(struct vm *vm) {
    return vm->program[vm->pc++];
}

// Reset the virtual machine
void vm_reset(struct vm *vm) {
    vm->status = STATUS_RESET;

    vm->pc = 0;
    vm->sp = 0;
    vm->a = 0;
    vm->b = 0;

    return;
}

void vm_run(struct vm *vm) {
    uint16_t instr;

    while (vm->status == STATUS_IS_RUNNING) {
        instr = vm_next_opcode(vm);
        vm_exec(vm, instr);
    }

    return;
}
