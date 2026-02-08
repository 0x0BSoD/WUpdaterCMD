# Testing WUpdaterCMD in Your VM

This guide provides step-by-step instructions for testing the modernized WUpdaterCMD in your Windows VM.

## Pre-Testing Checklist

### System Requirements
- [ ] Windows 10, Windows 11, or Windows Server 2016+
- [ ] Administrator access
- [ ] Internet connection (for downloading updates)
- [ ] At least 5GB free disk space

### Required Software
- [ ] Visual Studio 2019/2022 with "Desktop development with C++" workload
  - OR MinGW-w64 compiler
- [ ] CMake 3.15 or higher
- [ ] Git (optional, for cloning)

## Quick Start Testing

### Step 1: Transfer Files to VM

Transfer the entire `WUpdaterCMD` directory to your Windows VM. You can:
- Use shared folders
- USB drive
- Network share
- Git clone (if applicable)

### Step 2: Open Administrator Command Prompt

```batch
Right-click Start Menu → Windows PowerShell (Admin)
```

Or:

```batch
Right-click Start Menu → Command Prompt (Admin)
```

### Step 3: Navigate to Project Directory

```batch
cd C:\Path\To\WUpdaterCMD
```

### Step 4: Build the Project

#### Option A: Using PowerShell Script (Recommended)
```powershell
.\build.ps1
```

#### Option B: Using Batch Script
```batch
build.bat
```

#### Option C: Manual CMake
```batch
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
```

### Step 5: Verify Build Success

Check that the executable was created:

```batch
dir build\bin\Release\WUpdaterCMD.exe
```

You should see the file with its size and timestamp.

## Basic Functionality Tests

### Test 1: Help Command

Verify the program starts and displays help:

```batch
build\bin\Release\WUpdaterCMD.exe --help
```

**Expected Output:**
```
Usage: WUpdaterCMD.exe <option>
Options:
        -h, --help              Show this help message
        -q, --quiet             Run without asking for confirmation
        -c, --criteria PATH     Specify the path to file with search criteria
                                i.e. IsInstalled=0 and Type='Software' and IsHidden=0
```

**Status:** ✅ Pass / ❌ Fail

---

### Test 2: Search for Updates (Interactive)

Run a search with the default criteria file:

```batch
build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

**Expected Behavior:**
1. Program searches for updates
2. Lists found updates with release dates
3. Prompts: "Download updates? [y/n]:"
4. If you answer 'n', program exits gracefully

**What to Look For:**
- No crashes or errors
- Clear update information displayed
- Proper formatting of dates
- Correct count of updates

**Status:** ✅ Pass / ❌ Fail

---

### Test 3: Download Updates (Interactive)

```batch
build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

When prompted:
1. Answer 'y' to download
2. Answer 'n' to install (to avoid system changes during testing)

**Expected Behavior:**
- Progress messages during download
- Successful download confirmation for each update
- Program exits after declining installation

**Status:** ✅ Pass / ❌ Fail

---

### Test 4: Quiet Mode (Automation)

**⚠️ WARNING:** This will download AND install updates without prompting!

```batch
build\bin\Release\WUpdaterCMD.exe -c criteria.txt --quiet
```

**Expected Behavior:**
- No user prompts
- Automatic download of updates
- Automatic installation of updates
- Progress messages displayed
- Exit when complete

**Status:** ✅ Pass / ❌ Fail

---

### Test 5: Custom Criteria File

Create a custom criteria file:

```batch
echo IsInstalled=0 and Type='Software' and CategoryIDs contains '0FA1201D-4330-4FA8-8AE9-B877473B6441' > security-updates.txt
```

Run with custom criteria:

```batch
build\bin\Release\WUpdaterCMD.exe -c security-updates.txt
```

**Expected Behavior:**
- Reads custom criteria file
- Searches for security updates only
- Displays matching updates

**Status:** ✅ Pass / ❌ Fail

---

### Test 6: Error Handling - Missing Criteria File

```batch
build\bin\Release\WUpdaterCMD.exe -c nonexistent.txt
```

**Expected Behavior:**
- Error message about missing file
- Graceful exit without crash

**Status:** ✅ Pass / ❌ Fail

---

### Test 7: Error Handling - No Arguments

```batch
build\bin\Release\WUpdaterCMD.exe
```

**Expected Behavior:**
- Display usage information
- Exit with error code

**Status:** ✅ Pass / ❌ Fail

---

### Test 8: Ctrl+C Signal Handling

```batch
build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

During execution, press `Ctrl+C`

**Expected Behavior:**
- Catches interrupt signal
- Displays "Caught interrupt signal" message
- Exits gracefully

**Status:** ✅ Pass / ❌ Fail

---

## Advanced Testing

### Test 9: Run Example Automation Script

```powershell
.\example-automation.ps1 -SkipInstall
```

**Expected Behavior:**
- Checks prerequisites
- Runs WUpdaterCMD
- Creates log file
- Reports success/failure

**Status:** ✅ Pass / ❌ Fail

---

### Test 10: Already Downloaded Updates

If some updates are already downloaded:

```batch
build\bin\Release\WUpdaterCMD.exe -c criteria.txt
```

**Expected Behavior:**
- Marks already downloaded updates as "Already downloaded"
- Only adds new updates to download list
- Correct counting of updates to download

**Status:** ✅ Pass / ❌ Fail

---

### Test 11: No Updates Available

Create criteria for already installed updates:

```batch
echo IsInstalled=1 > installed.txt
build\bin\Release\WUpdaterCMD.exe -c installed.txt
```

**Expected Behavior:**
- Searches successfully
- Reports "No updates found"
- Exits gracefully

**Status:** ✅ Pass / ❌ Fail

---

## Performance Testing

### Test 12: Large Update Sets

Use broad criteria to find many updates:

```batch
echo Type='Software' > all-software.txt
build\bin\Release\WUpdaterCMD.exe -c all-software.txt
```

**What to Check:**
- No memory leaks (check Task Manager during execution)
- Reasonable performance with many updates
- All updates listed correctly
- No crashes

**Status:** ✅ Pass / ❌ Fail

---

## Regression Testing (vs Original Version)

If you have the old version, compare behavior:

| Feature | Old Version | New Version | Status |
|---------|-------------|-------------|--------|
| Search functionality | Works | ? | |
| Download updates | Works | ? | |
| Install updates | Works | ? | |
| Quiet mode | Works | ? | |
| Criteria parsing | Works | ? | |
| Error messages | Basic | Enhanced | |
| Memory usage | Higher | Lower | |

---

## VM Snapshot Strategy

**Before Testing:**
1. Create VM snapshot: "Before WUpdaterCMD Testing"
2. This allows you to rollback if needed

**After Successful Basic Tests:**
1. Create VM snapshot: "WUpdaterCMD Basic Tests Passed"

**After Full Installation Test:**
1. Create VM snapshot: "WUpdaterCMD Full Test Complete"

---

## Troubleshooting Test Failures

### Build Failures

**Issue:** CMake not found
```batch
# Install CMake or add to PATH
set PATH=%PATH%;C:\Program Files\CMake\bin
```

**Issue:** Compiler not found
```batch
# Use Developer Command Prompt for VS
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
```

**Issue:** Link errors
```batch
# Clean and rebuild
rmdir /s /q build
build.bat
```

### Runtime Failures

**Issue:** "Access Denied"
- Ensure running as Administrator
- Check antivirus hasn't blocked the executable

**Issue:** "COM initialization failed"
- Restart Windows Update service: `net stop wuauserv && net start wuauserv`
- Reboot the VM

**Issue:** "No updates found" but updates are available
- Check Windows Update service is running
- Verify criteria syntax
- Try with default criteria.txt

### Network Issues

**Issue:** "Operation did not complete because the network connection was unavailable"
- Check VM network connectivity
- Verify proxy settings if applicable
- Check Windows Update service can reach Microsoft servers

---

## Test Report Template

Copy this template and fill it out:

```
=== WUpdaterCMD Test Report ===

Date: _______________
Tester: _____________
VM Configuration:
  - OS: Windows _____
  - Version: _________
  - Build: ___________
  - RAM: _____ GB
  - Disk Space: _____ GB

Build Configuration:
  - Compiler: MSVC / MinGW
  - Build Type: Debug / Release
  - CMake Version: _______

Test Results:
  [ ] Test 1: Help Command
  [ ] Test 2: Search for Updates
  [ ] Test 3: Download Updates
  [ ] Test 4: Quiet Mode
  [ ] Test 5: Custom Criteria
  [ ] Test 6: Missing File Error
  [ ] Test 7: No Arguments Error
  [ ] Test 8: Ctrl+C Handling
  [ ] Test 9: Automation Script
  [ ] Test 10: Already Downloaded
  [ ] Test 11: No Updates
  [ ] Test 12: Large Update Sets

Issues Found:
1. ___________________________________
2. ___________________________________
3. ___________________________________

Overall Status: PASS / FAIL

Notes:
_______________________________________
_______________________________________
_______________________________________

Recommendations:
_______________________________________
_______________________________________
_______________________________________
```

---

## Safe Testing Practices

### DO's ✅
- ✅ Test in a VM, never on production systems first
- ✅ Create VM snapshots before testing
- ✅ Read all output messages carefully
- ✅ Start with small update sets
- ✅ Test interactive mode before quiet mode
- ✅ Check Windows Event Viewer for errors
- ✅ Monitor system resources during execution

### DON'Ts ❌
- ❌ Don't test on production systems without VM testing first
- ❌ Don't run quiet mode without understanding what it does
- ❌ Don't interrupt during installation phase
- ❌ Don't test without Administrator privileges
- ❌ Don't ignore error messages
- ❌ Don't skip creating backups/snapshots
- ❌ Don't test on systems with pending reboots

---

## Expected Improvements Over Original

The modernized version should demonstrate:

1. **Better Memory Management**
   - No memory leaks
   - Proper cleanup on errors
   - Smart pointers handle cleanup automatically

2. **Improved Error Handling**
   - More descriptive error messages
   - Better recovery from failures
   - Exception safety

3. **Enhanced User Experience**
   - Clearer progress messages
   - Better formatted output
   - More informative status updates

4. **Code Quality**
   - Easier to build with CMake
   - Works with multiple compilers
   - Modern C++ standards

---

## Next Steps After Testing

### If All Tests Pass ✅
1. Document your test results
2. Consider deploying to test environment
3. Create deployment package
4. Document any quirks or notes
5. Plan gradual rollout

### If Tests Fail ❌
1. Document the failure
2. Note the exact error message
3. Check TROUBLESHOOTING section in README.md
4. Review build output for warnings
5. Open an issue with:
   - Windows version
   - Compiler version
   - Exact error message
   - Steps to reproduce

---

## Success Criteria

The test is considered successful if:

- ✅ Program builds without errors
- ✅ All basic tests (1-8) pass
- ✅ No crashes or hangs occur
- ✅ Updates are correctly identified
- ✅ Downloads complete successfully
- ✅ Installations work as expected
- ✅ Error handling works properly
- ✅ No memory leaks observed
- ✅ Performance is acceptable

---

## Additional Resources

- Review README.md for usage details
- Check BUILDING.md for build troubleshooting
- See example-automation.ps1 for automation examples
- Consult CHANGELOG.md for what's new

---

## Support

If you encounter issues during testing:
1. Check this document's troubleshooting section
2. Review BUILDING.md for build issues
3. Check Windows Event Viewer (Windows Logs → Application)
4. Enable verbose logging if available
5. Document and report issues with full details

Good luck with your testing! 🚀