@echo off
echo =========================================
echo       Starting Quiz System Build...
echo =========================================

REM Build the C++ project using CMake
cmake --build build

REM Check if the build was successful
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Build failed! Please check the output above.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =========================================
echo       Build Successful! Starting Server...
echo =========================================
echo.

REM Run the compiled executable
.\build\QuizSystem.exe

pause
