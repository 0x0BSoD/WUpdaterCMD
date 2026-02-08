# Building WUpdaterCMD

This document provides detailed instructions for building WUpdaterCMD on Windows.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Build Methods](#build-methods)
  - [Method 1: Using Build Scripts](#method-1-using-build-scripts)
  - [Method 2: Manual CMake Build](#method-2-manual-cmake-build)
  - [Method 3: Visual Studio IDE](#method-3-visual-studio-ide)
  - [Method 4: MinGW](#method-4-mingw)
- [Build Types](#build-types)
- [Troubleshooting](#troubleshooting)
- [Testing the Build](#testing-the-build)

## Prerequisites

### Required

1. **Windows Operating System**
   - Windows 10 or later
   - Windows Server 2016 or later

2. **CMake** (Version 3.15 or higher)
   - Download from: https://cmake.org/download/
   - During installation, select "Add CMake to system PATH"

3. **C++ Compiler** (Choose one):
   - **Visual Studio 2019 or newer** (Recommended)
     - Download Community Edition: https://visualstudio.microsoft.com/
     - Install "Desktop development with C++" workload
   - **MinGW-w64** (Alternative)
     - Download from: https://sourceforge.net/projects/mingw-w64/

4. **Windows SDK**
   - Included with Visual Studio
   - Or download separately: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/

### Verify Prerequisites

Open Command Prompt or PowerShell and run:

```batch
cmake --version
```

For Visual Studio:
```batch
cl
```

For MinGW:
```batch
g++ --version
```

## Quick Start

The fastest way to build:

### Using Command Prompt
```batch
build.bat
```

### Using PowerShell
```powershell
.\build.ps1
```

The executable will be created in:
- `build/bin/Release/WUpdaterCMD.exe` (Release build)
- `build/bin/Debug/WUpdaterCMD.exe` (Debug build)

## Build Methods

### Method 1: Using Build Scripts

#### Windows Batch Script

**Release build (optimized, recommended):**
```batch
build.bat Release
```

**Debug build (with debug symbols):**
```batch
build.bat Debug
```

#### PowerShell Script

**Release build:**
```powershell
.\build.ps1 -BuildType Release
```

**Debug build:**
```powershell
.\build.ps1 -BuildType Debug
```

**Clean build (removes old build files):**
```powershell
.\build.ps1 -Clean
```

**Verbose output:**
```powershell
.\build.ps1 -Verbose
```

**Combined options:**
```powershell
.\build.ps1 -BuildType Release -Clean -Verbose
```

### Method 2: Manual CMake Build

#### Step-by-Step

1. **Create build directory:**
```batch
mkdir build
cd build
```

2. **Configure with CMake:**

For Visual Studio:
```batch
cmake .. -G "Visual Studio 16 2019" -A x64
```

For Visual Studio 2022:
```batch
cmake .. -G "Visual Studio 17 2022" -A x64
```

For MinGW:
```batch
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

3. **Build:**
```batch
cmake --build . --config Release
```

4. **Install (optional):**
```batch
cmake --install . --config Release --prefix ../install
```

### Method 3: Visual Studio IDE

1. **Generate Visual Studio solution:**
```batch
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```

2. **Open the solution:**
```batch
start WUpdaterCMD.sln
```

3. **In Visual Studio:**
   - Select build configuration (Debug/Release)
   - Press F7 or Build → Build Solution
   - Press F5 to build and run

### Method 4: MinGW

If you prefer MinGW over Visual Studio:

1. **Ensure MinGW is in PATH:**
```batch
where g++
where mingw32-make
```

2. **Configure and build:**
```batch
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make
```

Or use cmake to build:
```batch
cmake --build .
```

## Build Types

CMake supports several build types:

| Build Type | Description | Use Case |
|------------|-------------|----------|
| **Release** | Optimized, no debug info | Production use, fastest execution |
| **Debug** | No optimization, full debug info | Development, debugging |
| **RelWithDebInfo** | Optimized with debug info | Testing optimized builds with debugging |
| **MinSizeRel** | Optimized for size | Minimal executable size |

Specify build type during configuration:
```batch
cmake .. -DCMAKE_BUILD_TYPE=Release
```

For multi-config generators (Visual Studio), specify during build:
```batch
cmake --build . --config Release
```

## Troubleshooting

### CMake Not Found

**Error:** `'cmake' is not recognized as an internal or external command`

**Solution:**
1. Install CMake from https://cmake.org/download/
2. During installation, select "Add CMake to system PATH for all users"
3. Restart your terminal/command prompt
4. Verify: `cmake --version`

### Compiler Not Found

**Error:** `No CMAKE_CXX_COMPILER could be found`

**Solution for Visual Studio:**
1. Install Visual Studio with "Desktop development with C++" workload
2. Use "Developer Command Prompt for VS" or "Developer PowerShell for VS"
3. Or run: `"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"`

**Solution for MinGW:**
1. Install MinGW-w64
2. Add MinGW `bin` directory to PATH
3. Verify: `g++ --version`

### Windows SDK Not Found

**Error:** `Cannot find wuapi.h` or similar

**Solution:**
1. Install Windows SDK (included with Visual Studio)
2. Or download from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
3. Verify SDK location in Visual Studio Installer

### Build Fails with Linker Errors

**Error:** `unresolved external symbol` or similar

**Solutions:**
1. Ensure you're using 64-bit build: `-A x64` or `-DCMAKE_GENERATOR_PLATFORM=x64`
2. Clean and rebuild:
   ```batch
   rmdir /s /q build
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

### Permission Denied

**Error:** Access denied when running executable

**Solution:**
1. Run as Administrator
2. Check antivirus hasn't quarantined the file
3. Ensure file isn't marked as blocked (Right-click → Properties → Unblock)

### Outdated CMake Version

**Error:** `CMake 3.15 or higher is required`

**Solution:**
1. Update CMake to latest version
2. Download from: https://cmake.org/download/

### Build Configuration Issues

If builds are inconsistent or failing:

1. **Clean build:**
```batch
rmdir /s /q build
```

2. **Check environment:**
```batch
echo %PATH%
where cmake
where cl
```

3. **Use developer command prompt:**
   - Start → "Developer Command Prompt for VS 2019"
   - Or "Developer PowerShell for VS 2019"

## Testing the Build

After successful build:

1. **Navigate to executable:**
```batch
cd build\bin\Release
```

2. **Test help:**
```batch
WUpdaterCMD.exe --help
```

3. **Test with criteria file:**
```batch
# Create test criteria file
echo IsInstalled=0 and Type='Software' and IsHidden=0 > test_criteria.txt

# Run (requires Administrator)
WUpdaterCMD.exe -c test_criteria.txt
```

4. **Test quiet mode:**
```batch
WUpdaterCMD.exe -c test_criteria.txt --quiet
```

## Advanced Build Options

### Custom Install Location

```batch
cmake --install build --prefix C:\Tools\WUpdaterCMD
```

### Building with Specific Toolchain

```batch
cmake .. -DCMAKE_CXX_COMPILER=cl
cmake .. -DCMAKE_CXX_COMPILER=g++
```

### Parallel Builds

Speed up compilation with parallel jobs:

```batch
cmake --build . --config Release -- /m
```

Or for MinGW:
```batch
cmake --build . -- -j8
```

### Static Analysis

Enable more warnings:

```batch
cmake .. -DCMAKE_CXX_FLAGS="/W4"
```

## Build Output

Successful build creates:

```
WUpdaterCMD/
├── build/
│   ├── bin/
│   │   ├── Release/
│   │   │   └── WUpdaterCMD.exe  ← Main executable
│   │   └── Debug/
│   │       └── WUpdaterCMD.exe
│   └── lib/                      ← Libraries (if any)
```

## Creating a Package

Build a distributable package:

```batch
cd build
cpack -C Release
```

This creates a ZIP package with the executable and documentation.

## Next Steps

After successful build:

1. Read [README.md](README.md) for usage instructions
2. Create your `criteria.txt` file
3. Run with Administrator privileges
4. Test in a non-production environment first

## Getting Help

If you encounter issues:

1. Check this document's [Troubleshooting](#troubleshooting) section
2. Verify all prerequisites are installed
3. Clean build and try again
4. Check CMake output for specific error messages
5. Open an issue on GitHub with:
   - Your Windows version
   - CMake version (`cmake --version`)
   - Compiler version
   - Complete error message
   - Build command used

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [Visual Studio Documentation](https://docs.microsoft.com/en-us/visualstudio/)
- [Windows Update Agent API](https://docs.microsoft.com/en-us/windows/win32/wua_sdk/portal-client)