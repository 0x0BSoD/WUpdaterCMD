#include "messages.h"
#include <sstream>
#include <iomanip>

namespace WUpdater {
namespace Messages {

    // Usage and help messages
    namespace Help {
        std::string getUsageMessage(const char* programName) {
            std::ostringstream oss;
            oss << "Usage: " << programName << " <option>\n"
                << "Options:\n"
                << "\t-h, --help\t\tShow this help message\n"
                << "\t-q, --quiet\t\tRun without asking for confirmation\n"
                << "\t-c, --criteria PATH\tSpecify the path to file with search criteria\n"
                << "\t\t\t\ti.e. IsInstalled=0 and Type='Software' and IsHidden=0\n";
            return oss.str();
        }

        std::wstring getWelcomeMessage() {
            return L"WUpdaterCMD - Windows Update Command Line Tool v2.0.0";
        }

        std::wstring getVersionInfo() {
            return L"Version 2.0.0 - Modern C++17 Edition";
        }
    }

    // Progress messages
    namespace Progress {
        std::wstring searchingUpdates() {
            return L"Searching for updates...";
        }

        std::wstring downloadingUpdates() {
            return L"Downloading updates...";
        }

        std::wstring installingUpdates() {
            return L"Installing updates...";
        }

        std::wstring operationComplete() {
            return L"Operation completed successfully!";
        }
    }

    // Status messages
    namespace Status {
        std::wstring noUpdatesFound() {
            return L"[!] No updates found";
        }

        std::wstring updatesFoundCount(long count) {
            std::wostringstream oss;
            oss << L"Found " << count << L" update" << (count != 1 ? L"s" : L"");
            return oss.str();
        }

        std::wstring alreadyDownloaded() {
            return L"Already downloaded";
        }

        std::wstring toDownload() {
            return L"To download";
        }

        std::wstring downloadComplete() {
            return L"Download completed";
        }

        std::wstring installationComplete() {
            return L"Installation completed";
        }
    }

    // Prompt messages
    namespace Prompts {
        std::wstring confirmDownload() {
            return L"Download updates? [y/n]: ";
        }

        std::wstring confirmInstall() {
            return L"Install updates? [y/n]: ";
        }

        std::wstring pressKeyToContinue() {
            return L"Press any key to continue...";
        }
    }

    // Error context messages
    namespace Errors {
        std::wstring criteriaFileNotFound(const std::string& path) {
            std::wostringstream oss;
            oss << L"[!] Unable to open criteria file: ";
            for (char c : path) {
                oss << static_cast<wchar_t>(c);
            }
            return oss.str();
        }

        std::wstring criteriaFileEmpty() {
            return L"[!] Criteria file is empty";
        }

        std::wstring comInitializationFailed() {
            return L"[!] Failed to initialize COM library";
        }

        std::wstring insufficientPrivileges() {
            return L"[!] This program requires Administrator privileges";
        }

        std::wstring serviceNotRunning() {
            return L"[!] Windows Update service is not running";
        }
    }

    // Operation result messages
    namespace Results {
        std::wstring operationNotStarted() {
            return L"The operation is not started";
        }

        std::wstring operationInProgress() {
            return L"The operation is in progress";
        }

        std::wstring operationSucceeded() {
            return L"Successfully completed";
        }

        std::wstring operationSucceededWithErrors() {
            return L"Completed with errors during the operation";
        }

        std::wstring operationFailed() {
            return L"The operation failed to complete";
        }

        std::wstring operationCancelled() {
            return L"The operation was canceled";
        }

        std::wstring getResultMessage(int resultCode) {
            switch (resultCode) {
                case 0: return operationNotStarted();
                case 1: return operationInProgress();
                case 2: return operationSucceeded();
                case 3: return operationSucceededWithErrors();
                case 4: return operationFailed();
                case 5: return operationCancelled();
                default: {
                    std::wostringstream oss;
                    oss << L"Unknown result code: " << resultCode;
                    return oss.str();
                }
            }
        }
    }

    // Information messages
    namespace Info {
        std::wstring updateListHeader() {
            return L"\nList of applicable updates:";
        }

        std::wstring downloadListHeader() {
            return L"\nDownload results:";
        }

        std::wstring installListHeader() {
            return L"\nInstallation results:";
        }

        std::wstring criteriaLoaded() {
            return L"Search criteria: ";
        }

        std::wstring operationCancelledByUser() {
            return L"Operation cancelled by user.";
        }
    }

} // namespace Messages
} // namespace WUpdater