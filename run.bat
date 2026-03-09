@echo off
setlocal enabledelayedexpansion

echo =========================================
echo       Starting Quiz System Build...
echo =========================================
echo.
echo [INFO] Checking dependencies...

where cmake >nul 2>nul
if errorlevel 1 goto missing_cmake

set HAS_COMPILER=0
set CMAKE_GENERATOR=

where cl >nul 2>nul
if errorlevel 1 (
   rem cl missing
) else (
   set HAS_COMPILER=1
)

where g++ >nul 2>nul
if errorlevel 1 (
   rem g++ missing
) else (
   set HAS_COMPILER=1
   if "!CMAKE_GENERATOR!"=="" (
       rem If cl is missing but g++ is present, default to MinGW Makefiles
       where cl >nul 2>nul
       if errorlevel 1 set CMAKE_GENERATOR=-G "MinGW Makefiles"
   )
)

if "!HAS_COMPILER!"=="0" goto missing_compiler

echo [INFO] All dependencies met.
echo.

if not exist build goto config_step
echo [INFO] Removing old build directory to start from scratch...
rmdir /s /q build

:config_step
echo [INFO] Configuring the project...
if "!CMAKE_GENERATOR!"=="" (
    cmake -S . -B build
) else (
    echo [INFO] Using CMake Generator: !CMAKE_GENERATOR!
    cmake -S . -B build !CMAKE_GENERATOR!
)
if errorlevel 1 goto failed_config

echo.
echo [INFO] Compiling and Building the project...
cmake --build build --config Release
if errorlevel 1 goto failed_build

echo.
echo =========================================
echo       Build Successful! Starting System...
echo =========================================
echo.

set EXE_FOUND=0
if exist "build\Release\QuizSystem.exe" (
    set EXE_PATH="build\Release\QuizSystem.exe"
    set EXE_FOUND=1
)
if exist "build\QuizSystem.exe" (
    set EXE_PATH="build\QuizSystem.exe"
    set EXE_FOUND=1
)
if exist "build\Debug\QuizSystem.exe" (
    set EXE_PATH="build\Debug\QuizSystem.exe"
    set EXE_FOUND=1
)

if "!EXE_FOUND!"=="0" goto missing_exe

!EXE_PATH!
if errorlevel 1 goto failed_run

echo.
echo [INFO] QuizSystem executed successfully.
goto end_script

:missing_cmake
echo [ERROR] CMake is not installed or not in your PATH.
echo Please install CMake from https://cmake.org/download/
pause
exit /b 1

:missing_compiler
echo [ERROR] No C++ compiler found.
echo Please install Visual Studio with C++ workload or MinGW (GCC).
pause
exit /b 1

:failed_config
echo.
echo [ERROR] CMake configuration failed!
pause
exit /b 1

:failed_build
echo.
echo [ERROR] Compile/Build failed!
pause
exit /b 1

:missing_exe
echo [ERROR] Could not find the compiled executable in the build directory!
pause
exit /b 1

:failed_run
echo.
echo [ERROR] QuizSystem exited with an error.
pause
exit /b 1

:end_script
pause
