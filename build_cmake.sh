#!/bin/bash
# Build script for Smart Task Management System using CMake

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=========================================${NC}"
echo -e "${GREEN}Smart Task Management System - CMake Build${NC}"
echo -e "${GREEN}=========================================${NC}"

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN_BUILD=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Usage: $0 [--debug] [--clean]"
            exit 1
            ;;
    esac
done

# Create build directory
BUILD_DIR="build"

if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${GREEN}Configuring CMake (Build Type: $BUILD_TYPE)...${NC}"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..

# Build
echo -e "${GREEN}Building project...${NC}"
cmake --build . --config "$BUILD_TYPE" -j$(nproc)

# Check results
echo -e "${GREEN}=========================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}=========================================${NC}"

if [ -f "bin/task_manager" ]; then
    echo -e "${GREEN}✓ Console executable: bin/task_manager${NC}"
fi

if [ -f "bin/task_manager_gui" ]; then
    echo -e "${GREEN}✓ GUI executable: bin/task_manager_gui${NC}"
else
    echo -e "${YELLOW}⚠ GUI executable not built (Qt may not be available)${NC}"
fi

echo ""
echo -e "${YELLOW}To run console version:${NC}"
echo -e "  cd build && ./bin/task_manager"
echo ""
echo -e "${YELLOW}To run GUI version:${NC}"
echo -e "  cd build && ./bin/task_manager_gui"
echo ""
