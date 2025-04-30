A chip8 emulator built in c++ using sdl3.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

*   A C++ compiler supporting C++17 or later (e.g., GCC, Clang, MSVC)
*   [CMake](https://cmake.org/download/) (version 3.15 or later recommended)
*   [Git](https://git-scm.com/downloads)
*   [vcpkg](https://github.com/microsoft/vcpkg) (for managing dependencies)

### Installing Dependencies

1.  **Clone vcpkg** (if you don't have it already):
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.bat  # On Windows
    # ./bootstrap-vcpkg.sh  # On Linux/macOS
    ```

2.  **Integrate vcpkg with your build system** (optional but recommended):
    ```bash
    ./vcpkg integrate install
    ```

3.  **Install SDL3:**
    ```bash
    ./vcpkg install sdl3:x64-windows # Or your target triplet (e.g., :x64-linux, :arm64-osx)
    ```

### Building the Emulator

1.  **Clone the repository:**
    ```bash
    git clone <repository-url> # Replace <repository-url> with the actual URL
    cd Chip8 # Or your project directory name
    ```

2.  **Configure the project using CMake:**
    Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
    Run CMake, pointing it to the vcpkg toolchain file:
    ```bash
    # Replace <path-to-vcpkg> with the actual path to your vcpkg installation
    cmake .. -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
    ```
    *Example on Windows:*
    ```bash
    cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
    ```

3.  **Build the project:**
    ```bash
    cmake --build .
    ```
    *(Alternatively, on Windows with Visual Studio, you can open the generated solution file in the `build` directory and build from there.)*

### Running

After a successful build, the executable (e.g., `Chip8.exe` on Windows or `Chip8` on Linux/macOS) will be located in the `build` directory (or a subdirectory like `build/Debug` or `build/Release` depending on your configuration).

```bash
./Chip8 # Or ./Debug/Chip8.exe etc.