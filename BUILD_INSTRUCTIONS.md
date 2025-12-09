# Build Instructions

This project supports building both **Console** and **GUI** versions using CMake.

## Prerequisites

### All Platforms
- CMake 3.16 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- SQLite3

### For GUI Application
- Qt 5.15+ or Qt 6.x with the following components:
  - Qt Core
  - Qt Gui  
  - Qt Qml
  - Qt Quick

## Building on Windows

### Option 1: Using PowerShell Script (Recommended)

```powershell
# Release build
.\build_cmake.ps1

# Debug build
.\build_cmake.ps1 -Debug

# Clean build
.\build_cmake.ps1 -Clean

# Specify Qt path if not in PATH
.\build_cmake.ps1 -QtPath "C:\Qt\6.x\mingw_64\bin"
```

### Option 2: Manual CMake Build

```powershell
# Create build directory
mkdir build
cd build

# Configure (use appropriate generator for your compiler)
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --config Release

# Executables will be in build/bin/
```

## Building on Linux/macOS

### Option 1: Using Shell Script (Recommended)

```bash
# Release build
./build_cmake.sh

# Debug build
./build_cmake.sh --debug

# Clean build
./build_cmake.sh --clean
```

### Option 2: Manual CMake Build

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . -j$(nproc)

# Executables will be in build/bin/
```

## Using the Legacy Makefile

The traditional Makefile is still available for console-only builds:

```bash
# Build console version
make

# Clean
make clean

# Run
make run
```

## Build Outputs

After successful build, you will find:

- **Console Application**: `build/bin/task_manager` (or `task_manager.exe` on Windows)
- **GUI Application**: `build/bin/task_manager_gui` (or `task_manager_gui.exe` on Windows)

## Troubleshooting

### GUI executable runs but doesn't show window (Windows)

This was the original issue! The CMakeLists.txt now includes:
- `WIN32` flag for `add_executable()` - prevents console window
- `WIN32_EXECUTABLE ON` property - sets Windows subsystem to GUI
- Proper resource compilation for QML files

Make sure to:
1. Build using CMake (not the old Makefile which doesn't support GUI)
2. Ensure Qt is properly installed and in PATH
3. Check that `sqlite3.dll` is in the same directory as the executable

### Qt not found

If CMake cannot find Qt:

**Windows:**
```powershell
# Add Qt to PATH before building
$env:PATH = "C:\Qt\6.x\mingw_64\bin;$env:PATH"
# Or use -QtPath parameter with build_cmake.ps1
```

**Linux:**
```bash
# Install Qt
sudo apt-get install qt6-base-dev qt6-declarative-dev  # Ubuntu/Debian
# or
sudo apt-get install qtbase5-dev qtdeclarative5-dev   # For Qt5
```

**macOS:**
```bash
# Install Qt via Homebrew
brew install qt6
# or
brew install qt@5
```

### Missing sqlite3.dll (Windows)

Copy `sqlite/sqlite3.dll` to the same directory as your executable:
```powershell
Copy-Item sqlite\sqlite3.dll build\bin\
```

## Project Structure

```
├── CMakeLists.txt          # Main build configuration
├── build_cmake.sh          # Linux/macOS build script
├── build_cmake.ps1         # Windows build script
├── Makefile                # Legacy Makefile (console only)
├── build.ps1               # Legacy PowerShell script (console only)
├── src/
│   ├── main.cpp            # Console application entry point
│   ├── main_gui.cpp        # GUI application entry point
│   └── ...                 # Other source files
├── include/                # Header files
├── resources/
│   ├── qml.qrc             # Qt resource file
│   └── qml/                # QML UI files
└── sqlite/                 # SQLite library
```

## Features

- **Console Version**: Full-featured terminal UI with gamification
- **GUI Version**: Modern QML-based interface with "悬赏令" (Bounty Board) theme
- **Cross-platform**: Builds on Windows, Linux, and macOS
- **Static Linking**: Console version uses static libraries for portability
- **Resource Embedding**: GUI version embeds QML files in executable
