#include <iostream>
#include <SDL3/SDL.h>
#include "Chip8.h"

#define PIXEL_SIZE 10 // Size of each pixel in the display

void draw(SDL_Surface* screenSurface); // Function to draw the display buffer to the SDL surface

Chip8 chip8; // Create an instance of the Chip8 class

// Key mapping for Chip-8
const int keyMapping[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", 640, 320, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Get the window surface
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    if (!screenSurface) {
        std::cerr << "Window surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    chip8.initialize(); // Initialize the Chip-8 emulator

    // Load a ROM file into the Chip-8 emulator
    chip8.loadRom("../../../Pong2.ch8");

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Emulate one cycle of the Chip-8 CPU
        chip8.emulateCycle();
        if (chip8.draw_flag) {
            // Clear the surface
            SDL_FillSurfaceRect(screenSurface, NULL, SDL_MapRGB(SDL_GetPixelFormatDetails(screenSurface->format), NULL, 0, 0, 0));
            // Draw the display buffer to the SDL surface
            draw(screenSurface);
            chip8.draw_flag = false; // Reset the draw flag
        }

        // Update the surface
        SDL_UpdateWindowSurface(window);

        // Set Key
        const bool* state = SDL_GetKeyboardState(NULL);
        for (int i = 0; i < 16; ++i) {
            chip8.setKey(i, state[keyMapping[i]]);
        }
        
        // Delay to control the speed of the emulation
        SDL_Delay(3);
    }

    // Clean up and close the window
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void draw(SDL_Surface* screenSurface) {
    // Draw the display buffer to the SDL surface
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (chip8.getPixel(x, y) == 1) {
                SDL_Rect rect = { x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
                // Fill the surface with a color
                SDL_FillSurfaceRect(screenSurface, &rect, SDL_MapRGB(SDL_GetPixelFormatDetails(screenSurface->format), NULL, 255, 255, 255));
            }
        }
    }
}
