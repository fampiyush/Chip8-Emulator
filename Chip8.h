#pragma once

class Chip8 {
    private:
        uint8_t memory[4096]; // 4KB of memory
        uint8_t V[16]; // 16 registers (V0 to VF)
        uint16_t I; // Index register
        uint16_t PC; // Program counter
        uint8_t delay_timer; // Delay timer
        uint8_t sound_timer; // Sound timer
        uint16_t stack[16]; // Stack for subroutine calls
        uint8_t SP; // Stack pointer
        uint8_t display[64 * 32]; // 64x32 pixel display
        bool keys[16]; // Keypad state (0x0 to 0xF)
        bool draw_flag; // Flag to indicate if the display needs to be updated
        uint8_t fontset[80]; // Fontset for the Chip-8

    public:
        void initialize(); // Initialize the Chip-8 system
        void loadRom(const char* filename); // Load a ROM into memory
        void emulateCycle(); // Emulate one cycle of the Chip-8 CPU
};