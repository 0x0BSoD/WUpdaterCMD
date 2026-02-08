#pragma once

#include <wuapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <ATLComTime.h>
#include <wuerror.h>
#include <comutil.h>
#include <comdef.h>

#include "error_messages.h"
#include "messages.h"

// COM smart pointer types for Windows Update API
_COM_SMARTPTR_TYPEDEF(IUpdateSession, __uuidof(IUpdateSession));
_COM_SMARTPTR_TYPEDEF(IUpdateSearcher, __uuidof(IUpdateSearcher));
_COM_SMARTPTR_TYPEDEF(ISearchResult, __uuidof(ISearchResult));
_COM_SMARTPTR_TYPEDEF(IUpdateCollection, __uuidof(IUpdateCollection));
_COM_SMARTPTR_TYPEDEF(IUpdate, __uuidof(IUpdate));
_COM_SMARTPTR_TYPEDEF(IUpdateDownloader, __uuidof(IUpdateDownloader));
_COM_SMARTPTR_TYPEDEF(IDownloadResult, __uuidof(IDownloadResult));
_COM_SMARTPTR_TYPEDEF(IUpdateDownloadResult, __uuidof(IUpdateDownloadResult));
_COM_SMARTPTR_TYPEDEF(IUpdateInstaller, __uuidof(IUpdateInstaller));
_COM_SMARTPTR_TYPEDEF(IInstallationResult, __uuidof(IInstallationResult));
_COM_SMARTPTR_TYPEDEF(IUpdateInstallationResult, __uuidof(IUpdateInstallationResult));
_COM_SMARTPTR_TYPEDEF(IDownloadJob, __uuidof(IDownloadJob));
_COM_SMARTPTR_TYPEDEF(IDownloadProgressChangedCallback, __uuidof(IDownloadProgressChangedCallback));
_COM_SMARTPTR_TYPEDEF(IDownloadCompletedCallback, __uuidof(IDownloadCompletedCallback));
_COM_SMARTPTR_TYPEDEF(IDownloadProgressChangedCallbackArgs, __uuidof(IDownloadProgressChangedCallbackArgs));
_COM_SMARTPTR_TYPEDEF(IDownloadCompletedCallbackArgs, __uuidof(IDownloadCompletedCallbackArgs));
_COM_SMARTPTR_TYPEDEF(IDownloadProgress, __uuidof(IDownloadProgress));

namespace WUpdater {

    // Progress callback phases
    enum class ProgressPhase {
        BEGIN = 0,
        SEARCHING = 1,
        DOWNLOADING = 2,
        INSTALLING = 3,
        END = 4
    };

    // Operation result codes
    enum class ResultCode {
        NOT_STARTED = 0,
        IN_PROGRESS = 1,
        SUCCEEDED = 2,
        SUCCEEDED_WITH_ERRORS = 3,
        FAILED = 4,
        ABORTED = 5
    };

    // Search session structure
    struct SearchSession {
        IUpdateSessionPtr session;
        IUpdateSearcherPtr searcher;
        ISearchResultPtr results;
    };

    // Update information structure
    struct UpdateInfo {
        IUpdateCollectionPtr updatesList;
        IUpdatePtr item;
        VARIANT_BOOL inCache;
        LONG size;
        _bstr_t name;
        DATE releaseDate;
        OperationResultCode resultCode;
    };

    // Command line parameters
    struct CommandLineArgs {
        std::string criteriaFilePath;
        bool quietMode = false;
    };

    // Forward declarations
    class UpdateManager;
    class ProgressCallback;

    // Function declarations
    void showUsage(const char* programName);
    int parseArguments(int argc, char* argv[], CommandLineArgs& params);
    _bstr_t getCriteriaFromFile(const std::string& filePath);
    int checkHResult(HRESULT hr);
    void signalHandler(int signal);

    // Update Manager class
    class UpdateManager {
    public:
        UpdateManager();
        ~UpdateManager();

        // Disable copy
        UpdateManager(const UpdateManager&) = delete;
        UpdateManager& operator=(const UpdateManager&) = delete;

        // Main operations
        int searchForUpdates(const _bstr_t& criteria);
        int printUpdateInfo(IUpdateCollectionPtr toDownloadList);
        int downloadUpdates(IUpdateCollectionPtr toDownloadList);
        int installUpdates();

        // Getters
        LONG getUpdateCount() const { return updateInfo_.size; }
        IUpdateCollectionPtr getUpdatesList() const { return updateInfo_.updatesList; }

    private:
        SearchSession search_;
        UpdateInfo updateInfo_;
        bool initialized_;

        void printResultCode(LONG index, const _bstr_t& name, ResultCode rc, const std::wstring& operation);
    };

    // Progress callback typedef
    typedef void (*UpdateProgressCallback)(ProgressPhase phase, UINT progress, void* context);

    // Default progress callback
    void updateProgressCallbackDefault(ProgressPhase phase, UINT progress, void* context);

    // COM callback base class template
    template <class InterfaceType>
    class ComCallbackBase : public InterfaceType {
    public:
        ComCallbackBase(UpdateProgressCallback callback, void* context)
            : callback_(callback), context_(context), refCount_(1) {}

        virtual ~ComCallbackBase() = default;

        // IUnknown implementation
        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override {
            if (ppvObject == nullptr)
                return E_POINTER;

            if (riid == __uuidof(IUnknown) || riid == __uuidof(InterfaceType)) {
                *ppvObject = this;
                AddRef();
                return S_OK;
            }

            return E_NOINTERFACE;
        }

        STDMETHODIMP_(ULONG) AddRef() override {
            return InterlockedIncrement(&refCount_);
        }

        STDMETHODIMP_(ULONG) Release() override {
            ULONG count = InterlockedDecrement(&refCount_);
            if (count == 0) {
                delete this;
            }
            return count;
        }

    protected:
        UpdateProgressCallback callback_;
        void* context_;
        LONG refCount_;
    };

    // Download progress callback implementation
    class DownloadProgressCallback : public ComCallbackBase<IDownloadProgressChangedCallback> {
    public:
        DownloadProgressCallback(UpdateProgressCallback callback, void* context)
            : ComCallbackBase(callback, context) {}

        STDMETHODIMP Invoke(IDownloadJob* job, IDownloadProgressChangedCallbackArgs* args) override;
    };

    // Download completed callback implementation
    class DownloadCompletedCallback : public ComCallbackBase<IDownloadCompletedCallback> {
    public:
        DownloadCompletedCallback(UpdateProgressCallback callback, void* context)
            : ComCallbackBase(callback, context) {
            event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        }

        ~DownloadCompletedCallback() override {
            if (event_) {
                CloseHandle(event_);
            }
        }

        STDMETHODIMP Invoke(IDownloadJob* job, IDownloadCompletedCallbackArgs* args) override;

        HANDLE GetEvent() const { return event_; }

    private:
        HANDLE event_;
    };

} // namespace WUpdater