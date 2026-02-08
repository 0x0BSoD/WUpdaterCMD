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
    std::cerr << "Usage: " << programName << " <option>\n"
              << "Options:\n"
              << "\t-h, --help\t\tShow this help message\n"
              << "\t-q, --quiet\t\tRun without asking for confirmation\n"
              << "\t-c, --criteria PATH\tSpecify the path to file with search criteria\n"
              << "\t\t\t\ti.e. IsInstalled=0 and Type='Software' and IsHidden=0\n"
              << std::endl;
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
        std::wcout << L"[!] Unable to open criteria file: " << filePath.c_str() << std::endl;
        return _bstr_t();
    }

    std::wcout << L"Search criteria: ";
    while (std::getline(file, criteriaStr)) {
        std::cout << criteriaStr << '\n';
    }
    file.close();

    if (criteriaStr.empty()) {
        std::wcout << L"[!] Criteria file is empty" << std::endl;
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

// Get error message for HRESULT
std::wstring WUpdater::getErrorMessage(HRESULT hr) {
    switch (hr) {
        case S_OK:
            return L"Success";
        case WU_E_NO_SERVICE:
            return L"WUA was unable to provide the service";
        case WU_E_MAX_CAPACITY_REACHED:
            return L"The maximum capacity of the service was exceeded";
        case WU_E_UNKNOWN_ID:
            return L"WUA cannot find an ID";
        case WU_E_NOT_INITIALIZED:
            return L"The object could not be initialized";
        case WU_E_RANGEOVERLAP:
            return L"The update handler requested a byte range overlapping a previously requested range";
        case WU_E_TOOMANYRANGES:
            return L"The requested number of byte ranges exceeds the maximum number";
        case WU_E_INVALIDINDEX:
            return L"The index to a collection was invalid";
        case WU_E_ITEMNOTFOUND:
            return L"The key for the item queried could not be found";
        case WU_E_OPERATIONINPROGRESS:
            return L"Another conflicting operation was in progress";
        case WU_E_COULDNOTCANCEL:
            return L"Cancellation of the operation was not allowed";
        case WU_E_CALL_CANCELLED:
            return L"Operation was cancelled";
        case WU_E_NOOP:
            return L"No operation was required";
        case WU_E_XML_MISSINGDATA:
            return L"WUA could not find required information in the update's XML data";
        case WU_E_XML_INVALID:
            return L"WUA found invalid information in the update's XML data";
        case WU_E_CYCLE_DETECTED:
            return L"Circular update relationships were detected in the metadata";
        case WU_E_TOO_DEEP_RELATION:
            return L"Update relationships too deep to evaluate were evaluated";
        case WU_E_INVALID_RELATIONSHIP:
            return L"An invalid update relationship was detected";
        case WU_E_REG_VALUE_INVALID:
            return L"An invalid registry value was read";
        case WU_E_DUPLICATE_ITEM:
            return L"Operation tried to add a duplicate item to a list";
        case WU_E_INVALID_INSTALL_REQUESTED:
            return L"Updates that are requested for install are not installable by the caller";
        case WU_E_INSTALL_NOT_ALLOWED:
            return L"Operation tried to install while another installation was in progress or the system was pending a mandatory restart";
        case WU_E_NOT_APPLICABLE:
            return L"Operation was not performed because there are no applicable updates";
        case WU_E_NO_USERTOKEN:
            return L"Operation failed because a required user token is missing";
        case WU_E_EXCLUSIVE_INSTALL_CONFLICT:
            return L"An exclusive update can't be installed with other updates at the same time";
        case WU_E_POLICY_NOT_SET:
            return L"A policy value was not set";
        case WU_E_SELFUPDATE_IN_PROGRESS:
            return L"The operation could not be performed because the Windows Update Agent is self-updating";
        case WU_E_INVALID_UPDATE:
            return L"An update contains invalid metadata";
        case WU_E_SERVICE_STOP:
            return L"Operation did not complete because the service or system was being shut down";
        case WU_E_NO_CONNECTION:
            return L"Operation did not complete because the network connection was unavailable";
        case WU_E_NO_INTERACTIVE_USER:
            return L"Operation did not complete because there is no logged-on interactive user";
        case WU_E_TIME_OUT:
            return L"Operation did not complete because it timed out";
        case WU_E_ALL_UPDATES_FAILED:
            return L"Operation failed for all the updates";
        case WU_E_EULAS_DECLINED:
            return L"The license terms for all updates were declined";
        case WU_E_NO_UPDATE:
            return L"There are no updates";
        case WU_E_USER_ACCESS_DISABLED:
            return L"Group Policy settings prevented access to Windows Update";
        case WU_E_INVALID_UPDATE_TYPE:
            return L"The type of update is invalid";
        case WU_E_URL_TOO_LONG:
            return L"The URL exceeded the maximum length";
        case WU_E_UNINSTALL_NOT_ALLOWED:
            return L"The update could not be uninstalled because the request did not originate from a WSUS server";
        case WU_E_INVALID_PRODUCT_LICENSE:
            return L"Search may have missed some updates because there is an unlicensed application on the system";
        case WU_E_MISSING_HANDLER:
            return L"A component required to detect applicable updates was missing";
        case WU_E_LEGACYSERVER:
            return L"An operation did not complete because it requires a newer version of server";
        case WU_E_BIN_SOURCE_ABSENT:
            return L"A delta-compressed update could not be installed because it required the source";
        case WU_E_SOURCE_ABSENT:
            return L"A full-file update could not be installed because it required the source";
        case WU_E_WU_DISABLED:
            return L"Access to an unmanaged server is not allowed";
        case WU_E_CALL_CANCELLED_BY_POLICY:
            return L"Operation did not complete because the DisableWindowsUpdateAccess policy was set";
        case WU_E_INVALID_PROXY_SERVER:
            return L"The format of the proxy list was invalid";
        case WU_E_INVALID_FILE:
            return L"The file is in the wrong format";
        case WU_E_INVALID_CRITERIA:
            return L"The search criteria string was invalid";
        case WU_E_EULA_UNAVAILABLE:
            return L"License terms could not be downloaded";
        case WU_E_DOWNLOAD_FAILED:
            return L"Update failed to download";
        case WU_E_UPDATE_NOT_PROCESSED:
            return L"The update was not processed";
        case WU_E_INVALID_OPERATION:
            return L"The object's current state did not allow the operation";
        case WU_E_NOT_SUPPORTED:
            return L"The functionality for the operation is not supported";
        case WU_E_TOO_MANY_RESYNC:
            return L"Agent is asked by server to resync too many times";
        case WU_E_NO_SERVER_CORE_SUPPORT:
            return L"The WUA API method does not run on the server core installation";
        case WU_E_SYSPREP_IN_PROGRESS:
            return L"Service is not available while sysprep is running";
        case WU_E_UNKNOWN_SERVICE:
            return L"The update service is no longer registered with automatic updates";
        case WU_E_NO_UI_SUPPORT:
            return L"No support for the WUA user interface";
        case WU_E_PER_MACHINE_UPDATE_ACCESS_DENIED:
            return L"Only administrators can perform this operation on per-computer updates";
        case WU_E_UNSUPPORTED_SEARCHSCOPE:
            return L"A search was attempted with a scope that is not currently supported";
        case WU_E_BAD_FILE_URL:
            return L"The URL does not point to a file";
        case WU_E_PT_WINHTTP_NAME_NOT_RESOLVED:
            return L"The proxy server or target server name cannot be resolved";
        case WU_E_INVALID_NOTIFICATION_INFO:
            return L"The featured update notification info returned by the server is invalid";
        case WU_E_OUTOFRANGE:
            return L"The data is out of range";
        case WU_E_SETUP_IN_PROGRESS:
            return L"WUA operations are not available while operating system setup is running";
        case WU_E_UNEXPECTED:
            return L"An operation failed due to reasons not covered by another error code";
        case CERT_E_EXPIRED:
            return L"A required certificate is not within its validity period";
        default:
            return L"Unknown error";
    }
}

// Check HRESULT and print error if needed
int WUpdater::checkHResult(HRESULT hr) {
    if (FAILED(hr)) {
        std::wcout << L"[!] Error code: 0x" << std::hex << hr << std::dec << std::endl;
        std::wcout << L"[!] " << getErrorMessage(hr) << std::endl;
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

        std::wcout << L"\nSearching for updates..." << std::endl;
        
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
    
    switch (rc) {
        case ResultCode::NOT_STARTED:
            std::wcout << L"The operation is not started" << std::endl;
            break;
        case ResultCode::IN_PROGRESS:
            std::wcout << L"The operation is in progress" << std::endl;
            break;
        case ResultCode::SUCCEEDED:
            std::wcout << L"Successfully " << operation << std::endl;
            break;
        case ResultCode::SUCCEEDED_WITH_ERRORS:
            std::wcout << L"Complete with errors during the operation" << std::endl;
            break;
        case ResultCode::FAILED:
            std::wcout << L"The operation failed to complete" << std::endl;
            break;
        case ResultCode::ABORTED:
            std::wcout << L"The operation is canceled" << std::endl;
            break;
        default:
            std::wcout << L"RESULT: " << static_cast<int>(rc) << std::endl;
            break;
    }
}

int UpdateManager::printUpdateInfo(IUpdateCollectionPtr toDownloadList) {
    if (!initialized_ || updateInfo_.updatesList == nullptr) {
        std::wcout << L"[!] No search has been performed" << std::endl;
        return -1;
    }

    if (updateInfo_.size == 0) {
        std::wcout << L"[!] No updates found" << std::endl;
        return -1;
    }

    std::wcout << L"\nList of applicable items on the machine:" << std::endl;

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
                std::wcout << L" | Already downloaded" << std::endl;
            } else {
                long newIndex;
                hr = toDownloadList->Add(updateInfo_.item, &newIndex);
                if (FAILED(hr)) {
                    std::wcout << L" | Error adding to download list" << std::endl;
                } else {
                    std::wcout << L" | To download" << std::endl;
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

        std::wcout << L"\nDownloading " << downloadCount << L" update(s)..." << std::endl;

        // Perform synchronous download
        IDownloadResultPtr downloadResult;
        hr = downloader->Download(&downloadResult);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Display results
        std::wcout << L"\nDownload results:" << std::endl;
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

        std::wcout << L"\nInstalling updates..." << std::endl;

        // Perform installation
        IInstallationResultPtr installResult;
        hr = installer->Install(&installResult);
        if (checkHResult(hr) != 0) {
            return -1;
        }

        // Display results
        std::wcout << L"\nInstallation results:" << std::endl;
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
        std::wcout << L"[!] Failed to initialize COM library. Error code: 0x" 
                  << std::hex << hr << std::dec << std::endl;
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
            std::wcout << L"\nNo updates available." << std::endl;
            goto cleanup;
        }

        // Ask for download confirmation (unless quiet mode)
        if (!args.quietMode && downloadCount > 0) {
            std::wcout << L"\nDownload updates? [y/n]: ";
            char input;
            std::cin >> input;
            if (input != 'y' && input != 'Y') {
                std::wcout << L"Download cancelled by user." << std::endl;
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
            std::wcout << L"\nInstall updates? [y/n]: ";
            char input;
            std::cin >> input;
            if (input != 'y' && input != 'Y') {
                std::wcout << L"Installation cancelled by user." << std::endl;
                goto cleanup;
            }
        }

        // Install updates
        if (manager.installUpdates() != 0) {
            exitCode = 1;
            goto cleanup;
        }

        std::wcout << L"\nUpdate process completed successfully!" << std::endl;

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