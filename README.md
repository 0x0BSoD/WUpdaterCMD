# WUpdaterCMD

A modern C++ command-line tool for automating Windows Update operations. This tool is designed for system administrators and automation scenarios where you need to programmatically search, download, and install Windows updates.

## Features

- 🔍 **Search for updates** using customizable criteria
- ⬇️ **Download updates** automatically or interactively
- 📦 **Install updates** with progress reporting
- 🤖 **Quiet mode** for automation scenarios
- 💻 **Modern C++17** codebase with smart pointers and proper memory management
- 🛠️ **CMake build system** for flexible compilation

## Requirements

- **Windows OS**: Windows 10, Windows 11, Windows Server 2016+
- **Compiler**: 
  - Visual Studio 2019 or newer (recommended)
  - MinGW-w64 with GCC 7.0+ (experimental)
- **CMake**: Version 3.15 or higher
- **Administrator privileges**: Required for installing updates

## Building the Project

### Using the Build Script (Easiest)

Simply run the provided batch file:

```batch
# Release build (default)
build.bat

# Debug build
build.bat Debug
```

### Manual CMake Build

```batch
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release
```

### Visual Studio

```batch
# Generate Visual Studio solution
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"

# Open the generated .sln file or build from command line
cmake --build . --config Release
```

## Usage

```batch
WUpdaterCMD.exe -c <criteria_file> [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-h`, `--help` | Show help message |
| `-c`, `--criteria PATH` | Specify the path to file with search criteria (required) |
| `-q`, `--quiet` | Run without asking for confirmation (for automation) |

### Examples

**Interactive mode with criteria file:**
```batch
WUpdaterCMD.exe -c criteria.txt
```

**Automated mode (no prompts):**
```batch
WUpdaterCMD.exe -c criteria.txt --quiet
```

## Search Criteria

Create a text file with Windows Update search criteria. The criteria uses the Windows Update Agent API query syntax.

### Example criteria.txt

```
IsInstalled=0 and Type='Software' and IsHidden=0
```

### Common Criteria Patterns

**All uninstalled software updates:**
```
IsInstalled=0 and Type='Software'
```

**Security updates only:**
```
IsInstalled=0 and Type='Software' and CategoryIDs contains '0FA1201D-4330-4FA8-8AE9-B877473B6441'
```

**Critical and security updates:**
```
IsInstalled=0 and Type='Software' and (CategoryIDs contains '0FA1201D-4330-4FA8-8AE9-B877473B6441' or CategoryIDs contains 'E6CF1350-C01B-414D-A61F-263D14D133B4')
```

**Updates not hidden:**
```
IsInstalled=0 and IsHidden=0
```

For more information on search criteria, see the [Microsoft documentation](https://docs.microsoft.com/en-us/windows/win32/api/wuapi/nf-wuapi-iupdatesearcher-search).

## Modernization Changes (2024)

This project has been modernized from the original 2019 version with the following improvements:

### Code Quality
- ✅ Updated to **C++17** standard
- ✅ Replaced raw pointers with **COM smart pointers** (`_com_ptr_t`)
- ✅ Proper **exception handling** with try-catch blocks
- ✅ **RAII principles** for resource management
- ✅ Better **error messages** and logging
- ✅ Organized code with **namespaces** (`WUpdater`)
- ✅ **Strong typing** with enums instead of magic numbers

### Build System
- ✅ Added **CMake** support for flexible building
- ✅ Cross-compiler support (MSVC, MinGW)
- ✅ Automated build scripts
- ✅ Proper dependency management

### Project Structure
- ✅ Added `.gitignore` for clean repository
- ✅ MIT License included
- ✅ Improved documentation
- ✅ Example configuration files

### Memory Management
- ✅ No memory leaks with smart pointers
- ✅ Automatic cleanup with RAII
- ✅ Proper COM reference counting

## Project Structure

```
WUpdaterCMD/
├── main.cpp          # Main implementation
├── main.h            # Header file with declarations
├── CMakeLists.txt    # CMake build configuration
├── build.bat         # Windows build script
├── criteria.txt      # Example search criteria
├── README.md         # This file
├── LICENSE           # MIT License
└── .gitignore        # Git ignore rules
```

## How It Works

1. **Initialization**: The tool initializes COM and creates a Windows Update session
2. **Search**: Searches for updates matching the criteria from your criteria file
3. **Display**: Lists all found updates with release dates and download status
4. **Download**: Downloads updates that aren't already cached (with optional confirmation)
5. **Install**: Installs the downloaded updates (with optional confirmation)
6. **Report**: Provides detailed results for each operation

## Troubleshooting

### Build Issues

**Error: CMake not found**
- Install CMake from https://cmake.org/download/
- Add CMake to your PATH environment variable

**Error: Cannot find wuapi.h**
- Ensure you have Windows SDK installed
- Install Visual Studio with "Desktop development with C++" workload

### Runtime Issues

**Error: Access Denied**
- Run the program as Administrator
- Windows Update operations require elevated privileges

**Error: No updates found**
- Check your search criteria syntax
- Ensure Windows Update service is running: `net start wuauserv`
- Verify network connectivity

**Error: COM initialization failed**
- Ensure no other update operations are running
- Restart Windows Update service

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## History

- **2024**: Modernized with C++17, CMake, smart pointers, and improved error handling
- **2019**: Original weekend project, built with Visual Studio 2019

## Acknowledgments

- Uses Windows Update Agent (WUA) API
- Built with modern C++ practices and COM smart pointers

## See Also

- [Windows Update Agent API Documentation](https://docs.microsoft.com/en-us/windows/win32/wua_sdk/portal-client)
- [CMake Documentation](https://cmake.org/documentation/)
- [COM Smart Pointers](https://docs.microsoft.com/en-us/cpp/cpp/com-ptr-t-class)

---

**Note**: This tool is intended for system administrators and advanced users. Always test in a non-production environment first and ensure you have proper backups before installing updates.