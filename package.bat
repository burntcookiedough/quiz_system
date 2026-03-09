@echo off
setlocal enabledelayedexpansion

echo =========================================
echo       Packaging Quiz System...
echo =========================================
echo.

:: 1. Locate the executable
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

if "!EXE_FOUND!"=="0" (
    echo [ERROR] Could not find the compiled executable!
    echo Please run 'run.bat' first to build the project.
    pause
    exit /b 1
)

:: 2. Create the staging directory
if exist dist rmdir /s /q dist
mkdir dist

:: 3. Copy executable
echo [INFO] Copying executable to staging folder...
copy !EXE_PATH! "dist\QuizSystem.exe" >nul

:: 4. Copy required assets
echo [INFO] Copying UI and Database assets...
if exist ui xcopy /E /I /Q /Y ui "dist\ui" >nul
if exist data xcopy /E /I /Q /Y data "dist\data" >nul

:: 5. Create the ZIP file
echo [INFO] Creating standalone QuizSystem_Release.zip...
if exist QuizSystem_Release.zip del QuizSystem_Release.zip
cd dist
tar -a -c -f ..\QuizSystem_Release.zip *
cd ..

:: 6. Cleanup the staging directory
rmdir /s /q dist

echo.
echo =========================================
echo       Packaging Complete!
echo =========================================
echo.
echo Your final standalone product is: QuizSystem_Release.zip
echo You can send this ZIP to anyone. They only need to extract it
echo and double-click "QuizSystem.exe" to start the server!
pause
