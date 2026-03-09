#!/bin/bash

echo "========================================="
echo "      Starting Quiz System Build...      "
echo "========================================="
echo ""

# 1. Check for CMake
echo "[INFO] Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "[ERROR] CMake is not installed or not in your PATH."
    echo "For Mac:   brew install cmake"
    echo "For Linux: sudo apt install cmake"
    exit 1
fi

# 2. Check for a C++ compiler (clang++ or g++)
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "[ERROR] No C++ compiler found."
    echo "For Mac:   xcode-select --install"
    echo "For Linux: sudo apt install build-essential"
    exit 1
fi

echo "[INFO] All dependencies met."
echo ""

# 3. Clean old build
if [ -d "build" ]; then
    echo "[INFO] Removing old build directory to start from scratch..."
    rm -rf build
fi

# 4. Configure project
echo "[INFO] Configuring the project..."
if ! cmake -S . -B build; then
    echo ""
    echo "[ERROR] CMake configuration failed!"
    exit 1
fi

# 5. Build project
echo ""
echo "[INFO] Compiling and Building the project..."
if ! cmake --build build --config Release; then
    echo ""
    echo "[ERROR] Compile/Build failed!"
    exit 1
fi

echo ""
echo "========================================="
echo "      Build Successful! Starting System..."
echo "========================================="
echo ""

# 6. Run executable
if [ ! -f "build/QuizSystem" ]; then
    echo "[ERROR] Could not find the compiled executable (build/QuizSystem)!"
    exit 1
fi

./build/QuizSystem
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
    echo ""
    echo "[ERROR] QuizSystem exited with an error."
    exit $EXIT_CODE
fi

echo ""
echo "[INFO] QuizSystem executed successfully."
exit 0
