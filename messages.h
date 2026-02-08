#pragma once

#include <string>

namespace WUpdater {
namespace Messages {

    // Usage and help messages
    namespace Help {
        std::string getUsageMessage(const char* programName);
        std::wstring getWelcomeMessage();
        std::wstring getVersionInfo();
    }

    // Progress messages
    namespace Progress {
        std::wstring searchingUpdates();
        std::wstring downloadingUpdates();
        std::wstring installingUpdates();
        std::wstring operationComplete();
    }

    // Status messages
    namespace Status {
        std::wstring noUpdatesFound();
        std::wstring updatesFoundCount(long count);
        std::wstring alreadyDownloaded();
        std::wstring toDownload();
        std::wstring downloadComplete();
        std::wstring installationComplete();
    }

    // Prompt messages
    namespace Prompts {
        std::wstring confirmDownload();
        std::wstring confirmInstall();
        std::wstring pressKeyToContinue();
    }

    // Error context messages
    namespace Errors {
        std::wstring criteriaFileNotFound(const std::string& path);
        std::wstring criteriaFileEmpty();
        std::wstring comInitializationFailed();
        std::wstring insufficientPrivileges();
        std::wstring serviceNotRunning();
    }

    // Operation result messages
    namespace Results {
        std::wstring operationNotStarted();
        std::wstring operationInProgress();
        std::wstring operationSucceeded();
        std::wstring operationSucceededWithErrors();
        std::wstring operationFailed();
        std::wstring operationCancelled();
        std::wstring getResultMessage(int resultCode);
    }

    // Information messages
    namespace Info {
        std::wstring updateListHeader();
        std::wstring downloadListHeader();
        std::wstring installListHeader();
        std::wstring criteriaLoaded();
        std::wstring operationCancelledByUser();
    }

} // namespace Messages
} // namespace WUpdater