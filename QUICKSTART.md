# Quick Start Guide - WUpdaterCMD

Get up and running in 5 minutes!

## 🚀 For the Impatient

```batch
# 1. Open PowerShell as Administrator
# 2. Navigate to project directory
cd C:\Path\To\WUpdaterCMD

# 3. Build
.\build.ps1

# 4. Test
.\build\bin\Release\WUpdaterCMD.exe --help

# 5. Run (will prompt for confirmation)
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

Done! ✅

---

## Step-by-Step (First Time)

### Prerequisites Check

Open PowerShell or Command Prompt and verify:

```batch
cmake --version          # Should show 3.15 or higher
```

If CMake is missing: https://cmake.org/download/

### 1. Get the Code

You already have it if you're reading this! If not:
- Copy the entire WUpdaterCMD folder to your Windows VM
- Or clone from Git if applicable

### 2. Open as Administrator

**Important:** You MUST run as Administrator!

- Right-click Start Menu
- Select "Windows PowerShell (Admin)" or "Command Prompt (Admin)"

### 3. Navigate to Project

```batch
cd C:\Path\To\WUpdaterCMD
```

### 4. Build

**Option A - PowerShell (Recommended):**
```powershell
.\build.ps1
```

**Option B - Command Prompt:**
```batch
build.bat
```

**Option C - Manual:**
```batch
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
```

Build takes ~30 seconds. You should see:
```
========================================
Build completed successfully!
========================================
```

### 5. Verify Build

```batch
dir build\bin\Release\WUpdaterCMD.exe
```

You should see the executable with size and date.

### 6. Test It

**Test help:**
```batch
.\build\bin\Release\WUpdaterCMD.exe --help
```

Should show usage information.

**Search for updates:**
```batch
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

This will:
1. Search for available updates
2. Show you what's available
3. Ask if you want to download (y/n)
4. Ask if you want to install (y/n)

Safe to test - you can say 'n' to avoid changes!

---

## Common First-Time Issues

### ❌ "cmake: command not found"
**Fix:** Install CMake from https://cmake.org/download/
Make sure to select "Add to PATH" during installation.

### ❌ "No compiler found"
**Fix:** Install Visual Studio with "Desktop development with C++" workload
Or use "Developer Command Prompt for VS"

### ❌ "Access Denied" when running
**Fix:** Run PowerShell/Command Prompt as Administrator
(Right-click → Run as Administrator)

### ❌ Build fails with errors
**Fix:** Try clean build:
```batch
rmdir /s /q build
.\build.ps1
```

---

## What the Files Do

| File | Purpose |
|------|---------|
| `main.cpp` | Main program code |
| `main.h` | Header definitions |
| `CMakeLists.txt` | Build configuration |
| `build.ps1` | PowerShell build script |
| `build.bat` | Batch build script |
| `criteria.txt` | Search criteria (what updates to find) |
| `README.md` | Full documentation |
| `BUILDING.md` | Detailed build help |
| `TESTING.md` | Complete test guide |

---

## Quick Test Scenarios

### 1. Safe Search (No Changes)
```batch
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt
# Answer 'n' when asked to download
```

### 2. Search Security Updates Only
```batch
echo IsInstalled=0 and CategoryIDs contains '0FA1201D-4330-4FA8-8AE9-B877473B6441' > security.txt
.\build\bin\Release\WUpdaterCMD.exe -c security.txt
```

### 3. Quiet Mode (Automated - Will Install!)
```batch
# WARNING: This will download AND install without prompting!
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt --quiet
```

---

## Understanding the Output

```
Searching for updates...
List of applicable items on the machine:
1 - Security Update for Windows (KB1234567) | Release: 2024-01-15 | To download
2 - Update for Windows Defender (KB7654321) | Release: 2024-01-14 | Already downloaded

Download updates? [y/n]:
```

**What it means:**
- Update 1: Found, needs download
- Update 2: Found, already downloaded, ready to install
- Prompt: Waiting for your decision

**Safe answers:**
- `n` = Exit without changes
- `y` = Proceed (downloads or installs depending on stage)

---

## Next Steps

After successful quick start:

1. **Read full docs** → [README.md](README.md)
2. **Customize criteria** → Edit `criteria.txt`
3. **Run full tests** → Follow [TESTING.md](TESTING.md)
4. **Automate** → Check [example-automation.ps1](example-automation.ps1)

---

## Cheat Sheet

```batch
# Build
.\build.ps1                                    # PowerShell
build.bat                                      # Command Prompt

# Run
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt         # Interactive
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt --quiet # Automated
.\build\bin\Release\WUpdaterCMD.exe --help                  # Help

# Clean build
rmdir /s /q build
.\build.ps1

# Different build types
.\build.ps1 -BuildType Debug              # Debug build
.\build.ps1 -BuildType Release            # Release build (default)
.\build.ps1 -Clean                        # Clean before build
```

---

## 5-Minute VM Test Plan

Perfect for first-time testing in a VM:

**Time: 0:00 - Setup**
```batch
cd C:\Path\To\WUpdaterCMD
```

**Time: 0:30 - Build**
```batch
.\build.ps1
```

**Time: 1:00 - Test Help**
```batch
.\build\bin\Release\WUpdaterCMD.exe --help
```

**Time: 1:30 - Search Updates**
```batch
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt
# Press 'n' when prompted
```

**Time: 3:00 - Take VM Snapshot**
```
Name: "WUpdaterCMD - Basic Tests Passed"
```

**Time: 3:30 - Test Full Run (Optional)**
```batch
.\build\bin\Release\WUpdaterCMD.exe -c criteria.txt
# Press 'y' to download
# Press 'y' to install
```

**Time: 5:00 - Done!**

---

## Success Indicators

You know it's working when:

✅ Build completes without errors  
✅ Executable file exists in `build\bin\Release\`  
✅ Help command shows usage information  
✅ Search finds updates (if any available)  
✅ No crashes or error dialogs  

---

## Emergency Rollback

If something goes wrong:

1. **VM Snapshot**: Restore to snapshot before testing
2. **Build Issues**: Delete `build` folder and rebuild
3. **Runtime Issues**: Restart Windows Update service:
   ```batch
   net stop wuauserv
   net start wuauserv
   ```

---

## Getting Help

- **Build problems** → [BUILDING.md](BUILDING.md)
- **Usage questions** → [README.md](README.md)
- **Testing guide** → [TESTING.md](TESTING.md)
- **What changed** → [CHANGELOG.md](CHANGELOG.md)

---

## Pro Tips 💡

1. **Always test in VM first** - Never run on production without testing
2. **Create VM snapshots** - Before and after testing
3. **Read the output** - The program tells you what it's doing
4. **Start with 'n'** - Decline downloads first time to see what's available
5. **Check criteria.txt** - Customize what updates to search for

---

## That's It!

You're now ready to use WUpdaterCMD. For detailed usage, automation, and advanced features, check out the other documentation files.

**Happy updating! 🎉**

---

**Quick Links:**
- [Full Documentation](README.md)
- [Build Help](BUILDING.md)  
- [Testing Guide](TESTING.md)
- [Automation Example](example-automation.ps1)

**Project Status:** ✅ Ready for testing in VM
