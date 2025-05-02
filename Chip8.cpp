#include <iostream>
#include <cstdint>
#include <fstream>
#include "Chip8.h"

void Chip8::initialize() {
    // Initialize memory, registers, stack, etc.
    PC = 0x200; // Program starts at 0x200
    I = 0;
    SP = 0;
    delay_timer = 0;
    sound_timer = 0;
    draw_flag = false;

    // Load fontset into memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }

    // Clear display
    for (int i = 0; i < 64 * 32; ++i) {
        display[i] = 0;
    }
}

void Chip8::loadRom(const char* filename) {
    // Load a ROM file into memory starting at 0x200
    std::ifstream rom(filename, std::ios::binary);
    if (!rom) {
        // Handle error
        std::cerr << "Failed to open ROM file: " << filename << std::endl;
        return;
    }

    rom.read(reinterpret_cast<char*>(&memory[0x200]), 4096 - 0x200); // Load ROM into memory
}

void Chip8::emulateCycle() {
    // Fetch, decode, and execute the opcode
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1]; // Fetch opcode
    PC += 2; // Move to the next opcode

    // Decode and execute the opcode (this is just a placeholder)
    switch(opcode & 0xF000) {
        case 0x0000:
            // Handle 0x00E0 (clear screen) and 0x00EE (return from subroutine)
            if(opcode == 0x00E0) {
                // Clear the display
                for (int i = 0; i < 64 * 32; ++i) {
                    display[i] = 0;
                }
                draw_flag = true; // Set draw flag to update display
            } else if (opcode == 0x00EE) {
                // Return from subroutine
                --SP;
                PC = stack[SP];
            }
            break;
        case 0x1000:
            // Handle jump instruction
            PC = opcode & 0x0FFF; // Jump to address NNN
            break;
        case 0x2000:
            // Handle call subroutine instruction
            stack[SP] = PC; // Push current PC to stack
            ++SP;
            PC = opcode & 0x0FFF; // Jump to address NNN
            break;
        case 0x3000:
        case 0x4000:
            // Handle skip if equal/skip if not equal instruction
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                PC += 2; // Skip next instruction
            }
            break;
        case 0x5000:
            // Handle skip if registers are equal instruction
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                PC += 2; // Skip next instruction
            }
            break;
        case 0x6000:
            // Handle load value into register instruction
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF; // Load value into register
            break;
        case 0x7000:
            // Handle adding value to register instruction
            V[(opcode & 0x0f00) >> 8] += opcode & 0x00FF;
            break;
        case 0x8000:
            switch(opcode & 0x000f) {
                case 0x0000:
                    // Sets Vx to Vy
                    V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0001:
                    // Sets Vx to Vx | Vy
                    V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x0f00) >> 8] | V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0002:
                    // Sets Vx to Vx & Vy
                    V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x0f00) >> 8] & V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0003:
                    // Sets Vx to Vx ^ Vy
                    V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x0f00) >> 8] ^ V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0004:
                    // Adds Vy to Vx. VF is set to 1 when there's a carry, and to 0 when there isn't
                    if(V[(opcode & 0x00f0) >> 4] > (0xFF - V[(opcode & 0x0f00) >> 8])) {
                        V[0xF] = 1; // Set carry flag
                    } else {
                        V[0xF] = 0; // Clear carry flag
                    }
                    V[(opcode & 0x0f00) >> 8] += V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0005:
                    // Substracts Vy from Vx. VF is set to 0 when underflow else 1
                    if(V[(opcode & 0x00f0) >> 4] > (V[(opcode & 0x0f00) >> 8])) {
                        V[0xF] = 0;
                    }else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0f00) >> 8] -= V[(opcode & 0x00f0) >> 4];
                    break;
                case 0x0006:
                    // Shifts Vx to the right by 1, stores least significant bit to VF.
                    V[0xF] = V[(opcode & 0x0f00) >> 8] & 0x1;
                    V[(opcode & 0x0f00) >> 8] >>= 1;
                    break;
                case 0x0007:
                    // Subtracts Vx from Vy and stores it in Vx. VF is set to 0 when underflow else 1
                    if((V[(opcode & 0x0f00) >> 8]) > V[(opcode & 0x00f0) >> 4]) {
                        V[0xF] = 0;
                    }else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x00f0) >> 4] - V[(opcode & 0x0f00) >> 8];
                    break;
                case 0x000E:
                    // Shifts Vx to the left by 1, ssets VF to 1 if most significant bit is 1 else 0.
                    V[0xF] = (V[(opcode & 0x0f00) >> 8] & 0x80) >> 7;
                    V[(opcode & 0x0f00) >> 8] <<= 1;
                    break;
            }
        default:
            // Unknown opcode
            std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}