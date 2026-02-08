# Code Organization

This document explains the organization and architecture of the WUpdaterCMD codebase after the separation of concerns refactoring.

## Overview

The codebase follows a clean separation between **logic**, **data**, and **presentation**. This makes the code more maintainable, testable, and easier to internationalize.

---

## File Structure

```
WUpdaterCMD/
├── main.cpp                    # Main program logic and entry point
├── main.h                      # Core declarations and class definitions
├── error_messages.cpp          # Windows Update error message implementations
├── error_messages.h            # Error message function declarations
├── messages.cpp                # UI/user-facing message implementations
├── messages.h                  # UI message function declarations
├── CMakeLists.txt              # Build configuration
└── criteria.txt                # Example search criteria
```

---

## Module Breakdown

### 1. Core Logic (`main.cpp` / `main.h`)

**Purpose**: Contains the business logic and program flow.

**Responsibilities**:
- Windows Update API interactions
- COM object management
- Update search, download, and installation logic
- Command-line argument parsing
- Main program flow

**Key Classes**:
- `UpdateManager`: Encapsulates update operations
- `DownloadProgressCallback`: COM callback for download progress
- `DownloadCompletedCallback`: COM callback for download completion

**Does NOT contain**:
- User-facing strings
- Error message text
- UI prompts

---

### 2. Error Messages (`error_messages.cpp` / `error_messages.h`)

**Purpose**: Centralized Windows Update error code to human-readable message mapping.

**Namespace**: `WUpdater::ErrorMessages`

**Key Functions**:

```cpp
std::wstring getErrorMessage(HRESULT hr);
// Returns: Human-readable error message for any Windows Update error code
// Example: WU_E_NO_CONNECTION → "Operation did not complete because the 
//          network connection was unavailable"

std::wstring getErrorCategory(HRESULT hr);
// Returns: Category name (Network, Permission, Service, etc.)
// Example: WU_E_NO_CONNECTION → "Network"

bool isRecoverableError(HRESULT hr);
// Returns: true if the operation can be retried
// Example: WU_E_NO_CONNECTION → true (can retry when network is back)
//          WU_E_INVALID_CRITERIA → false (won't succeed on retry)
```

**Features**:
- 60+ Windows Update error codes mapped
- Categorized errors (Network, Permission, Service, Data, etc.)
- Recoverable vs non-recoverable error identification
- Uses `std::unordered_map` for O(1) lookup performance

**Benefits**:
- Easy to update error messages
- Single source of truth for error text
- Enables future internationalization
- Testable independently

---

### 3. UI Messages (`messages.cpp` / `messages.h`)

**Purpose**: All user-facing strings and prompts.

**Namespace**: `WUpdater::Messages`

**Sub-namespaces**:

#### `Messages::Help`
Help and usage information
```cpp
std::string getUsageMessage(const char* programName);
std::wstring getWelcomeMessage();
std::wstring getVersionInfo();
```

#### `Messages::Progress`
Progress indication messages
```cpp
std::wstring searchingUpdates();      // "Searching for updates..."
std::wstring downloadingUpdates();    // "Downloading updates..."
std::wstring installingUpdates();     // "Installing updates..."
std::wstring operationComplete();     // "Operation completed successfully!"
```

#### `Messages::Status`
Status information messages
```cpp
std::wstring noUpdatesFound();
std::wstring updatesFoundCount(long count);
std::wstring alreadyDownloaded();
std::wstring toDownload();
std::wstring downloadComplete();
std::wstring installationComplete();
```

#### `Messages::Prompts`
User interaction prompts
```cpp
std::wstring confirmDownload();       // "Download updates? [y/n]: "
std::wstring confirmInstall();        // "Install updates? [y/n]: "
std::wstring pressKeyToContinue();
```

#### `Messages::Errors`
High-level error context messages
```cpp
std::wstring criteriaFileNotFound(const std::string& path);
std::wstring criteriaFileEmpty();
std::wstring comInitializationFailed();
std::wstring insufficientPrivileges();
std::wstring serviceNotRunning();
```

#### `Messages::Results`
Operation result descriptions
```cpp
std::wstring operationNotStarted();
std::wstring operationInProgress();
std::wstring operationSucceeded();
std::wstring operationSucceededWithErrors();
std::wstring operationFailed();
std::wstring operationCancelled();
std::wstring getResultMessage(int resultCode);
```

#### `Messages::Info`
Informational messages
```cpp
std::wstring updateListHeader();
std::wstring downloadListHeader();
std::wstring installListHeader();
std::wstring criteriaLoaded();
std::wstring operationCancelledByUser();
```

---

## Design Principles

### 1. Separation of Concerns

**Before**:
```cpp
// Logic and strings mixed together
std::wcout << L"Downloading " << count << L" update(s)..." << std::endl;
```

**After**:
```cpp
// Logic separate from presentation
std::wcout << Messages::Progress::downloadingUpdates() 
           << L" (" << count << L" update(s))" << std::endl;
```

### 2. Single Responsibility

Each module has ONE responsibility:
- **main.cpp**: Program logic
- **error_messages.cpp**: Error code → message mapping
- **messages.cpp**: UI strings

### 3. Namespace Organization

Namespaces prevent naming conflicts and organize related functions:
```
WUpdater::
├── ErrorMessages::
│   ├── getErrorMessage()
│   ├── getErrorCategory()
│   └── isRecoverableError()
└── Messages::
    ├── Help::*
    ├── Progress::*
    ├── Status::*
    ├── Prompts::*
    ├── Errors::*
    ├── Results::*
    └── Info::*
```

### 4. Consistency

All user-facing strings come from `Messages` namespace:
- Easy to find all UI text
- Consistent formatting
- Easy to change wording across entire application

---

## Usage Examples

### Example 1: Displaying Error Messages

**Old Way** (strings in logic):
```cpp
std::wcout << L"[!] The update could not be uninstalled..." << std::endl;
```

**New Way** (separated):
```cpp
std::wcout << L"[!] " << ErrorMessages::getErrorMessage(hr) << std::endl;
// Also shows category:
std::wcout << L"[!] Category: " << ErrorMessages::getErrorCategory(hr) << std::endl;
// Check if we can retry:
if (ErrorMessages::isRecoverableError(hr)) {
    // Implement retry logic
}
```

### Example 2: User Prompts

**Old Way**:
```cpp
std::wcout << L"\nDownload updates? [y/n]: ";
```

**New Way**:
```cpp
std::wcout << L"\n" << Messages::Prompts::confirmDownload();
```

### Example 3: Progress Messages

**Old Way**:
```cpp
std::wcout << L"Searching for updates..." << std::endl;
```

**New Way**:
```cpp
std::wcout << Messages::Progress::searchingUpdates() << std::endl;
```

---

## Benefits of This Architecture

### 1. Maintainability ✅
- Change messages without touching logic
- All strings in one place
- Easy to review all user-facing text

### 2. Internationalization Ready 🌍
- Create `messages_fr.cpp` for French
- Create `messages_de.cpp` for German
- Select at runtime based on locale

### 3. Testability 🧪
- Mock message functions for unit tests
- Test logic without worrying about exact string output
- Test message functions independently

### 4. Consistency 📏
- All error messages follow same format
- All prompts follow same style
- Easy to enforce standards

### 5. Code Clarity 📖
- Logic code is cleaner and more readable
- Intent is clear: `confirmDownload()` vs literal string
- Easier for new developers to understand

### 6. Flexibility 🔧
- Add message variations easily
- Support different verbosity levels
- Dynamic messages with parameters

---

## Adding New Messages

### Step 1: Add to Header (`messages.h`)

```cpp
namespace Messages {
namespace Status {
    std::wstring updatesPending(long count);  // New message
}
}
```

### Step 2: Implement in Source (`messages.cpp`)

```cpp
namespace Messages {
namespace Status {
    std::wstring updatesPending(long count) {
        std::wostringstream oss;
        oss << count << L" update" << (count != 1 ? L"s" : L"") 
            << L" pending installation";
        return oss.str();
    }
}
}
```

### Step 3: Use in Main Code (`main.cpp`)

```cpp
long pending = getPendingCount();
std::wcout << Messages::Status::updatesPending(pending) << std::endl;
```

---

## Adding New Error Messages

### Update `error_messages.cpp`

Add to the `errorMap`:

```cpp
static const std::unordered_map<HRESULT, ErrorInfo> errorMap = {
    // ... existing entries ...
    
    { NEW_ERROR_CODE, { 
        L"Description of what went wrong",  // message
        L"ErrorCategory",                   // category
        true                                // recoverable?
    }},
};
```

That's it! The new error will automatically work with:
- `getErrorMessage()`
- `getErrorCategory()`
- `isRecoverableError()`

---

## Performance Considerations

### Error Message Lookup
- Uses `std::unordered_map` for O(1) lookup
- Static map initialized once at program start
- Negligible performance impact

### Memory Usage
- All strings are static/compile-time constants
- No dynamic allocation during normal operation
- ~20KB for all message strings combined

---

## Future Enhancements

### Planned Improvements

1. **Logging Integration**
   ```cpp
   Logger::log(Messages::Progress::searchingUpdates());
   ```

2. **Verbosity Levels**
   ```cpp
   Messages::setVerbosity(VerbosityLevel::Detailed);
   ```

3. **Internationalization**
   ```cpp
   Messages::setLocale("fr-FR");
   ```

4. **Formatted Messages**
   ```cpp
   Messages::format(L"Found {0} updates in {1} seconds", count, time);
   ```

5. **Message Templates**
   ```cpp
   MessageTemplate::format("SEARCH_COMPLETE", {count, duration});
   ```

---

## Migration Guide

If you have custom modifications to the old code:

### Before (Old Code)
```cpp
if (hr == WU_E_NO_CONNECTION) {
    std::wcout << L"Network connection unavailable" << std::endl;
}
```

### After (New Code)
```cpp
if (FAILED(hr)) {
    std::wcout << ErrorMessages::getErrorMessage(hr) << std::endl;
}
```

### Key Changes
1. Replace hardcoded strings with `Messages::*` functions
2. Replace error switch statements with `ErrorMessages::getErrorMessage()`
3. Use namespaced functions instead of literals

---

## Best Practices

### DO ✅
- Use message functions for all user-facing text
- Add new messages to appropriate namespace
- Keep message functions simple (no business logic)
- Document new message functions
- Use wide strings (wstring) for Unicode support

### DON'T ❌
- Don't hardcode strings in `main.cpp`
- Don't put business logic in message functions
- Don't duplicate messages (use parameters instead)
- Don't mix narrow and wide strings without conversion
- Don't use `Messages` for debug/internal logging

---

## Summary

The separated architecture provides:
- **Cleaner code**: Logic is separate from presentation
- **Easier maintenance**: Change messages without touching logic
- **Better organization**: Related messages grouped together
- **Future-proof**: Ready for internationalization
- **Professional**: Follows industry best practices

All user-facing strings are now centralized in `messages.cpp` and `error_messages.cpp`, making the codebase more maintainable and professional.

---

**Last Updated**: 2024  
**Version**: 2.0.0  
**Status**: Production Ready