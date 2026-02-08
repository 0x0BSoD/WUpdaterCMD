@echo off
REM Build script for WUpdaterCMD using CMake

setlocal

REM Set default build type
set BUILD_TYPE=Release
if not "%1"=="" set BUILD_TYPE=%1

echo ========================================
echo Building WUpdaterCMD
echo Build Type: %BUILD_TYPE%
echo ========================================
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake not found in PATH!
    echo Please install CMake and add it to your PATH.
    exit /b 1
)

REM Create build directory
if not exist build (
    echo Creating build directory...
    mkdir build
)

REM Navigate to build directory
cd build

REM Configure CMake
echo.
echo Configuring project with CMake...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    cd ..
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build . --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    cd ..
    exit /b 1
)

REM Success message
echo.
echo ========================================
echo Build completed successfully!
echo.
echo Executable location:
if "%BUILD_TYPE%"=="Debug" (
    echo   build\bin\Debug\WUpdaterCMD.exe
) else (
    echo   build\bin\Release\WUpdaterCMD.exe
)
echo ========================================

cd ..
endlocal
