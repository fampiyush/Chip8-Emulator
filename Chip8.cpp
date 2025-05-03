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

    // Decode and execute the opcode
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
            // Handle skip if equal instruction
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                PC += 2; // Skip next instruction
            }
            break;
        case 0x4000:
            // Handle skip if not equal instruction
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
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
                default:
                    std::cerr << "Unknown 0x8000 opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
        case 0x9000:
            // Handle skip if registers are not equal instruction
            if(V[(opcode & 0x0f00) >> 8] != V[(opcode & 0x00f0) >> 4]) {
                PC += 2;
            }
            break;
        case 0xA000:
            // Handle load address into index register instruction
            I = (opcode & 0x0fff);
            break;
        case 0xB000:
            // Handle jump to address plus V0 instruction
            PC = V[0x0] + (opcode & 0x0fff);
            break;
        case 0xC000:
            // Handle random number instruction
            V[(opcode & 0x0f00) >> 8] = (rand() % 256) & (opcode & 0x00ff); // Random number ANDed with NN
            break;
        case 0xD000:
            // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            uint8_t xCoord = V[(opcode & 0x0f00) >> 8];
            uint8_t yCoord = V[(opcode & 0x00f0) >> 4];
            uint8_t height = (opcode & 0x000f);

            V[0xF] = 0; // Reset collision flag

            for(int y=0; y<height; y++) {
                if(I + y >= 4096) {
                    std::cerr << "Sprite data out of bounds" << std::endl;
                    break;
                }
                uint8_t pixels = memory[I+y];
                for(int x=0; x<8; x++) {
                    if((pixels & (0x80 >> x)) != 0) {
                        int index = ((xCoord + x) % 64) + ((yCoord + y) % 32) * 64;
                        if(display[index] == 1) {
                            V[0xF] = 1; // Collision detected
                        }
                        display[index] ^= 1; // XOR to flip pixel
                    }
                }
            }

            draw_flag = true;
            break;
        case 0xE000:
            if((opcode & 0x000F) == 0x0001) {
                // Skip next instruction if key with value of Vx is pressed
                if(keys[V[(opcode & 0x0f00) >> 8]]) {
                    PC += 2;
                }
            } else if((opcode & 0x000F) == 0x000E) {
                // Skip next instruction if key with value of Vx is not pressed
                if(!keys[V[(opcode & 0x0f00) >> 8]]) {
                    PC += 2;
                }
            } else {
                std::cerr << "Unknown E000 opcode: " << std::hex << opcode << std::endl;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x0007:
                    // Load delay timer value into Vx
                    V[(opcode & 0x0f00) >> 8] = delay_timer;
                    break;
                case 0x000A:
                    // Wait for a key press and store the value in Vx
                    // This is a blocking call, so it will wait until a key is pressed
                    bool key_pressed = false;
                    for(int i = 0; i < 16; ++i) {
                        if(keys[i]) {
                            V[(opcode & 0x0f00) >> 8] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if(!key_pressed) {
                        PC -= 2; // Skip this instruction if no key was pressed
                    }
                    break;
                case 0x0015:
                    // Set delay timer to Vx
                    delay_timer = V[(opcode & 0x0f00) >> 8];
                    break;
                case 0x0018:
                    // Set sound timer to Vx
                    sound_timer = V[(opcode & 0x0f00) >> 8];
                    break;
                case 0x001E:
                    // Add Vx to I
                    I += V[(opcode & 0x0f00) >> 8];
                    break;
                case 0x0029:
                    // Set I to the location of the sprite for the character in Vx
                    I = V[(opcode & 0x0f00) >> 8] * 5; // Each character is 5 bytes
                    break;
                case 0x0033:
                    // Store BCD representation of Vx in memory locations I, I+1, and I+2
                    memory[I]     = V[(opcode & 0x0f00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0f00) >> 8] / 10) % 10;
                    memory[I + 2] = V[(opcode & 0x0f00) >> 8] % 10;
                    break;
                case 0x0055:
                    // Store registers V0 to Vx in memory starting
                    for(int i = 0; i <= ((opcode & 0x0f00) >> 8); ++i) {
                        memory[I + i] = V[i];
                    }
                    I += ((opcode & 0x0f00) >> 8) + 1; // Increment I after storing
                    break;
                case 0x0065:
                    // Fill registers V0 to Vx with values from memory starting at I
                    for(int i = 0; i <= ((opcode & 0x0f00) >> 8); ++i) {
                        V[i] = memory[I + i];
                    }
                    I += ((opcode & 0x0f00) >> 8) + 1; // Increment I after loading
                    break;
                default:
                    std::cerr << "Unknown F000 opcode: " << std::hex << opcode << std::endl;
                    break;
            }
            break;
        default:
            // Unknown opcode
            std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }

    // Update timers
    if(delay_timer > 0) {
        --delay_timer;
    }
    if(sound_timer > 0) {
        if(sound_timer == 1) {
            // Beep sound
            std::cout << "Beep!" << std::endl;
        }
        --sound_timer;
    }
}