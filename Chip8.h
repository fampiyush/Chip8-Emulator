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
        uint8_t fontset[80] =  // Fontset for the Chip-8
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        
        public:
        bool draw_flag; // Flag to indicate if the display needs to be updated

        void initialize(); // Initialize the Chip-8 system
        void loadRom(const char* filename); // Load a ROM into memory
        void emulateCycle(); // Emulate one cycle of the Chip-8 CPU
        void setKey(int key, bool state); // Set the state of a key
        uint8_t getPixel(int x, int y); // Get the pixel value at (x, y)
};