#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_DCOM

#include "main.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <signal.h>
#include <cstdlib>

using namespace WUpdater;

// Global flag for signal handling
volatile bool g_interrupted = false;

// Signal handler implementation
void WUpdater::signalHandler(int signal) {
    if (signal == SIGINT) {
        std::wcout << L"\n[!] Caught interrupt signal. Exiting..." << std::endl;
    } else {
        std::wcout << L"\n[!] Caught signal " << signal << std::endl;
    }
    g_interrupted = true;
    exit(1);
}

// Show usage information
void WUpdater::showUsage(const char* programName) {
    std::cerr << Messages::Help::getUsageMessage(programName);
}

// Parse command line arguments
int WUpdater::parseArguments(int argc, char* argv[], CommandLineArgs& params) {
    if (argc < 2) {
        showUsage(argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return -1;
        } else if (arg == "-c" || arg == "--criteria") {
            if (i + 1 < argc) {
                i++;
                params.criteriaFilePath = argv[i];
            } else {
                std::cerr << "[!] --criteria option requires one argument." << std::endl;
                return -1;
            }
        } else if (arg == "-q" || arg == "--quiet") {
            params.quietMode = true;
        } else {
            std::cerr << "[!] Unknown option: " << arg << std::endl;
            showUsage(argv[0]);
            return -1;
        }
    }

    if (params.criteriaFilePath.empty()) {
        std::cerr << "[!] Criteria file path is required. Use -c option." << std::endl;
        return -1;
    }

    return 0;
}

// Get search criteria from file
_bstr_t WUpdater::getCriteriaFromFile(const std::string& filePath) {
    std::string criteriaStr;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::wcout << Messages::Errors::criteriaFileNotFound(filePath) << std::endl;
        return _bstr_t();
    }

    std::wcout << Messages::Info::criteriaLoaded();
    while (std::getline(file, criteriaStr)) {
        std::cout << criteriaStr << '\n';
    }
    file.close();

    if (criteriaStr.empty()) {
        std::wcout << Messages::Errors::criteriaFileEmpty() << std::endl;
        return _bstr_t();
    }

    return _bstr_t(criteriaStr.c_str());
}

// Default progress callback
void WUpdater::updateProgressCallbackDefault(ProgressPhase phase, UINT progress, void* context) {
    switch (phase) {
        case ProgressPhase::BEGIN:
            std::wcout << L"Progress: Begin " << progress << std::endl;
            break;
        case ProgressPhase::SEARCHING:
            std::wcout << L"Progress: Searching " << progress << std::endl;
            break;
        case ProgressPhase::DOWNLOADING:
            std::wcout << L"Progress: Downloading " << progress << L"%" << std::endl;
            break;
        case ProgressPhase::INSTALLING:
            std::wcout << L"Progress: Installing " << progress << std::endl;
            break;
        case ProgressPhase::END:
            std::wcout << L"Progress: End " << progress << std::endl;
            break;
        default:
            std::wcout << L"Progress: Unknown phase" << std::endl;
            break;
    }
}



// Check HRESULT and print error if needed
int WUpdater::checkHResult(HRESULT hr) {
    if (FAILED(hr)) {
        std::wcout << L"[!] Error code: 0x" << std::hex << hr << std::dec << std::endl;
        std::wcout << L"[!] " << ErrorMessages::getErrorMessage(hr) << std::endl;
        return -1;
    }
    return 0;
}

// UpdateManager implementation
UpdateManager::UpdateManager() : initialized_(false) {
    updateInfo_.updatesList = nullptr;
    updateInfo_.item = nullptr;
    updateInfo_.size = 0;
    updateInfo_.inCache = VARIANT_FALSE;
    updateInfo_.releaseDate = 0;
    updateInfo_.resultCode = orcNotStarted;
}

UpdateManager::~UpdateManager() {
    // Smart pointers will handle cleanup automatically
}

int UpdateManager::searchForUpdates(const _bstr_t& criteria) {
    try {
        // Create update session
        HRESULT hr = search_.session.CreateInstance(CLSID_UpdateSession);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Create update searcher
        hr = search_.session->CreateUpdateSearcher(&search_.searcher);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        std::wcout << L"\n" << Messages::Progress::searchingUpdates() << std::endl;
        
        // Search for updates
        hr = search_.searcher->Search(criteria, &search_.results);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Get the update collection
        hr = search_.results->get_Updates(&updateInfo_.updatesList);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Get the count
        hr = updateInfo_.updatesList->get_Count(&updateInfo_.size);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        initialized_ = true;
        return 0;

    } catch (_com_error& e) {
        std::wcout << L"[!] COM Error: " << e.ErrorMessage() << std::endl;
        return -1;
    } catch (...) {
        std::wcout << L"[!] Unknown error during search" << std::endl;
        return -1;
    }
}

void UpdateManager::printResultCode(LONG index, const _bstr_t& name, ResultCode rc, const std::wstring& operation) {
    std::wcout << index + 1 << L" - " << (const wchar_t*)name << L" | ";
    
    if (rc == ResultCode::SUCCEEDED) {
        std::wcout << L"Successfully " << operation << std::endl;
    } else {
        std::wcout << Messages::Results::getResultMessage(static_cast<int>(rc)) << std::endl;
    }
}

int UpdateManager::printUpdateInfo(IUpdateCollectionPtr toDownloadList) {
    if (!initialized_ || updateInfo_.updatesList == nullptr) {
        std::wcout << L"[!] No search has been performed" << std::endl;
        return -1;
    }

    if (updateInfo_.size == 0) {
        std::wcout << Messages::Status::noUpdatesFound() << std::endl;
        return -1;
    }

    std::wcout << Messages::Info::updateListHeader() << std::endl;

    try {
        for (LONG i = 0; i < updateInfo_.size; i++) {
            HRESULT hr = updateInfo_.updatesList->get_Item(i, &updateInfo_.item);
            if (checkHResult(hr) != 0) {
                continue;
            }

            BSTR titleBstr = nullptr;
            hr = updateInfo_.item->get_Title(&titleBstr);
            if (FAILED(hr)) {
                continue;
            }
            updateInfo_.name = titleBstr;
            SysFreeString(titleBstr);

            hr = updateInfo_.item->get_LastDeploymentChangeTime(&updateInfo_.releaseDate);
            COleDateTime releaseDateTime;
            releaseDateTime.m_dt = updateInfo_.releaseDate;

            hr = updateInfo_.item->get_IsDownloaded(&updateInfo_.inCache);
            if (checkHResult(hr) != 0) {
                continue;
            }

            std::wcout << i + 1 << L" - " << (const wchar_t*)updateInfo_.name
                      << L" | Release: " << (LPCTSTR)releaseDateTime.Format(_T("%Y-%m-%d"));

            if (updateInfo_.inCache) {
                std::wcout << L" | " << Messages::Status::alreadyDownloaded() << std::endl;
            } else {
                long newIndex;
                hr = toDownloadList->Add(updateInfo_.item, &newIndex);
                if (FAILED(hr)) {
                    std::wcout << L" | Error adding to download list" << std::endl;
                } else {
                    std::wcout << L" | " << Messages::Status::toDownload() << std::endl;
                }
            }
        }
        return 0;

    } catch (_com_error& e) {
        std::wcout << L"[!] COM Error: " << e.ErrorMessage() << std::endl;
        return -1;
    } catch (...) {
        std::wcout << L"[!] Unknown error during update info printing" << std::endl;
        return -1;
    }
}

int UpdateManager::downloadUpdates(IUpdateCollectionPtr toDownloadList) {
    try {
        LONG downloadCount = 0;
        HRESULT hr = toDownloadList->get_Count(&downloadCount);
        if (checkHResult(hr) != 0 || downloadCount == 0) {
            std::wcout << L"No updates to download" << std::endl;
            return 0;
        }

        // Create downloader
        IUpdateDownloaderPtr downloader;
        hr = search_.session->CreateUpdateDownloader(&downloader);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        hr = downloader->put_Updates(toDownloadList);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        std::wcout << L"\n" << Messages::Progress::downloadingUpdates() << L" (" << downloadCount << L" update(s))" << std::endl;

        // Perform synchronous download
        IDownloadResultPtr downloadResult;
        hr = downloader->Download(&downloadResult);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Display results
        std::wcout << Messages::Info::downloadListHeader() << std::endl;
        for (LONG i = 0; i < downloadCount; i++) {
            IUpdatePtr update;
            hr = toDownloadList->get_Item(i, &update);
            if (FAILED(hr)) continue;

            BSTR titleBstr = nullptr;
            hr = update->get_Title(&titleBstr);
            if (FAILED(hr)) continue;
            _bstr_t title(titleBstr, false);

            IUpdateDownloadResultPtr updateResult;
            hr = downloadResult->GetUpdateResult(i, &updateResult);
            if (FAILED(hr)) continue;

            OperationResultCode resultCode;
            hr = updateResult->get_ResultCode(&resultCode);
            if (FAILED(hr)) continue;

            printResultCode(i, title, static_cast<ResultCode>(resultCode), L"downloaded");
        }

        return 0;

    } catch (_com_error& e) {
        std::wcout << L"[!] COM Error: " << e.ErrorMessage() << std::endl;
        return -1;
    } catch (...) {
        std::wcout << L"[!] Unknown error during download" << std::endl;
        return -1;
    }
}

int UpdateManager::installUpdates() {
    if (!initialized_ || updateInfo_.updatesList == nullptr) {
        std::wcout << L"[!] No updates to install" << std::endl;
        return -1;
    }

    try {
        // Create installer
        IUpdateInstallerPtr installer;
        HRESULT hr = installer.CreateInstance(CLSID_UpdateInstaller);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        hr = installer->put_Updates(updateInfo_.updatesList);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        std::wcout << L"\n" << Messages::Progress::installingUpdates() << std::endl;

        // Perform installation
        IInstallationResultPtr installResult;
        hr = installer->Install(&installResult);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Display results
        std::wcout << Messages::Info::installListHeader() << std::endl;
        for (LONG i = 0; i < updateInfo_.size; i++) {
            IUpdatePtr update;
            hr = updateInfo_.updatesList->get_Item(i, &update);
            if (FAILED(hr)) continue;

            BSTR titleBstr = nullptr;
            hr = update->get_Title(&titleBstr);
            if (FAILED(hr)) continue;
            _bstr_t title(titleBstr, false);

            IUpdateInstallationResultPtr updateResult;
            hr = installResult->GetUpdateResult(i, &updateResult);
            if (FAILED(hr)) continue;

            OperationResultCode resultCode;
            hr = updateResult->get_ResultCode(&resultCode);
            if (FAILED(hr)) continue;

            printResultCode(i, title, static_cast<ResultCode>(resultCode), L"installed");
        }

        return 0;

    } catch (_com_error& e) {
        std::wcout << L"[!] COM Error: " << e.ErrorMessage() << std::endl;
        return -1;
    } catch (...) {
        std::wcout << L"[!] Unknown error during installation" << std::endl;
        return -1;
    }
}

// Download progress callback implementation
STDMETHODIMP DownloadProgressCallback::Invoke(IDownloadJob* job, IDownloadProgressChangedCallbackArgs* args) {
    try {
        IDownloadProgressPtr progress;
        HRESULT hr = args->get_Progress(&progress);
        if (FAILED(hr)) {
            return hr;
        }

        LONG percent = 0;
        hr = progress->get_PercentComplete(&percent);
        if (SUCCEEDED(hr) && callback_) {
            callback_(ProgressPhase::DOWNLOADING, static_cast<UINT>(percent), context_);
        }

        return S_OK;
    } catch (...) {
        return E_FAIL;
    }
}

// Download completed callback implementation
STDMETHODIMP DownloadCompletedCallback::Invoke(IDownloadJob* job, IDownloadCompletedCallbackArgs* args) {
    try {
        if (callback_) {
            callback_(ProgressPhase::DOWNLOADING, 100, context_);
        }
        if (event_) {
            SetEvent(event_);
        }
        return S_OK;
    } catch (...) {
        return E_FAIL;
    }
}

// Main function
int main(int argc, char* argv[]) {
    // Register signal handler
    signal(SIGINT, signalHandler);

    // Parse command line arguments
    CommandLineArgs args;
    if (parseArguments(argc, argv, args) != 0) {
        return 1;
    }

    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::wcout << Messages::Errors::comInitializationFailed() 
                  << L". Error code: 0x" << std::hex << hr << std::dec << std::endl;
        return 1;
    }

    int exitCode = 0;

    try {
        // Get search criteria
        _bstr_t criteria = getCriteriaFromFile(args.criteriaFilePath);
        if (criteria.length() == 0) {
            exitCode = 1;
            goto cleanup;
        }

        // Create update manager
        UpdateManager manager;

        // Search for updates
        if (manager.searchForUpdates(criteria) != 0) {
            exitCode = 1;
            goto cleanup;
        }

        // Create download list
        IUpdateCollectionPtr toDownloadList;
        hr = toDownloadList.CreateInstance(CLSID_UpdateCollection);
        if (checkHResult(hr) != 0) {
            exitCode = 1;
            goto cleanup;
        }

        // Print update information
        if (manager.printUpdateInfo(toDownloadList) != 0) {
            exitCode = 1;
            goto cleanup;
        }

        // Check if there are updates to download
        LONG downloadCount = 0;
        toDownloadList->get_Count(&downloadCount);

        if (downloadCount == 0 && manager.getUpdateCount() == 0) {
            std::wcout << L"\n" << Messages::Status::noUpdatesFound() << std::endl;
            goto cleanup;
        }

        // Ask for download confirmation (unless quiet mode)
        if (!args.quietMode && downloadCount > 0) {
            std::wcout << L"\n" << Messages::Prompts::confirmDownload();
            char input;
            std::cin >> input;
            if (input != 'y' && input != 'Y') {
                std::wcout << Messages::Info::operationCancelledByUser() << std::endl;
                goto cleanup;
            }
        }

        // Download updates
        if (downloadCount > 0) {
            if (manager.downloadUpdates(toDownloadList) != 0) {
                exitCode = 1;
                goto cleanup;
            }
        }

        // Ask for installation confirmation (unless quiet mode)
        if (!args.quietMode) {
            std::wcout << L"\n" << Messages::Prompts::confirmInstall();
            char input;
            std::cin >> input;
            if (input != 'y' && input != 'Y') {
                std::wcout << Messages::Info::operationCancelledByUser() << std::endl;
                goto cleanup;
            }
        }

        // Install updates
        if (manager.installUpdates() != 0) {
            exitCode = 1;
            goto cleanup;
        }

        std::wcout << L"\n" << Messages::Progress::operationComplete() << std::endl;

    } catch (_com_error& e) {
        std::wcout << L"[!] COM Error: " << e.ErrorMessage() << std::endl;
        exitCode = 1;
    } catch (std::exception& e) {
        std::wcout << L"[!] Exception: " << e.what() << std::endl;
        exitCode = 1;
    } catch (...) {
        std::wcout << L"[!] Unknown error occurred" << std::endl;
        exitCode = 1;
    }

cleanup:
    CoUninitialize();
    return exitCode;
}