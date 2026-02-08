#include "error_messages.h"
#include <wuerror.h>
#include <unordered_map>

namespace WUpdater {
namespace ErrorMessages {

    // Error message mapping structure
    struct ErrorInfo {
        std::wstring message;
        std::wstring category;
        bool recoverable;
    };

    // Static map of all Windows Update error codes
    static const std::unordered_map<HRESULT, ErrorInfo> errorMap = {
        // Success
        { S_OK, { L"Operation completed successfully", L"Success", true } },

        // Service errors
        { WU_E_NO_SERVICE, { L"Windows Update Agent was unable to provide the service", L"Service", true } },
        { WU_E_SERVICE_STOP, { L"Operation did not complete because the service or system was being shut down", L"Service", true } },
        { WU_E_UNKNOWN_SERVICE, { L"The update service is no longer registered with automatic updates", L"Service", true } },

        // Capacity and limits
        { WU_E_MAX_CAPACITY_REACHED, { L"The maximum capacity of the service was exceeded", L"Capacity", false } },
        { WU_E_TOOMANYRANGES, { L"The requested number of byte ranges exceeds the maximum number", L"Capacity", false } },

        // ID and index errors
        { WU_E_UNKNOWN_ID, { L"Windows Update Agent cannot find an ID", L"Data", false } },
        { WU_E_INVALIDINDEX, { L"The index to a collection was invalid", L"Data", false } },
        { WU_E_ITEMNOTFOUND, { L"The key for the item queried could not be found", L"Data", false } },

        // Initialization errors
        { WU_E_NOT_INITIALIZED, { L"The object could not be initialized", L"Initialization", true } },

        // Range errors
        { WU_E_RANGEOVERLAP, { L"The update handler requested a byte range overlapping a previously requested range", L"Data", false } },

        // Operation state errors
        { WU_E_OPERATIONINPROGRESS, { L"Another conflicting operation was in progress. Some operations such as installation cannot be performed twice simultaneously", L"State", true } },
        { WU_E_COULDNOTCANCEL, { L"Cancellation of the operation was not allowed", L"State", false } },
        { WU_E_CALL_CANCELLED, { L"Operation was cancelled", L"Cancelled", true } },
        { WU_E_NOOP, { L"No operation was required", L"State", true } },

        // XML errors
        { WU_E_XML_MISSINGDATA, { L"Windows Update Agent could not find required information in the update's XML data", L"Data", false } },
        { WU_E_XML_INVALID, { L"Windows Update Agent found invalid information in the update's XML data", L"Data", false } },

        // Relationship errors
        { WU_E_CYCLE_DETECTED, { L"Circular update relationships were detected in the metadata", L"Data", false } },
        { WU_E_TOO_DEEP_RELATION, { L"Update relationships too deep to evaluate were evaluated", L"Data", false } },
        { WU_E_INVALID_RELATIONSHIP, { L"An invalid update relationship was detected", L"Data", false } },

        // Registry errors
        { WU_E_REG_VALUE_INVALID, { L"An invalid registry value was read", L"Configuration", false } },

        // Collection errors
        { WU_E_DUPLICATE_ITEM, { L"Operation tried to add a duplicate item to a list", L"Data", false } },

        // Installation errors
        { WU_E_INVALID_INSTALL_REQUESTED, { L"Updates that are requested for install are not installable by the caller", L"Installation", false } },
        { WU_E_INSTALL_NOT_ALLOWED, { L"Operation tried to install while another installation was in progress or the system was pending a mandatory restart", L"Installation", true } },
        { WU_E_NOT_APPLICABLE, { L"Operation was not performed because there are no applicable updates", L"Installation", true } },
        { WU_E_EXCLUSIVE_INSTALL_CONFLICT, { L"An exclusive update can't be installed with other updates at the same time", L"Installation", true } },

        // Permission errors
        { WU_E_NO_USERTOKEN, { L"Operation failed because a required user token is missing", L"Permission", false } },
        { WU_E_PER_MACHINE_UPDATE_ACCESS_DENIED, { L"Only administrators can perform this operation on per-computer updates", L"Permission", false } },
        { WU_E_USER_ACCESS_DISABLED, { L"Group Policy settings prevented access to Windows Update", L"Permission", false } },

        // Policy errors
        { WU_E_POLICY_NOT_SET, { L"A policy value was not set", L"Policy", false } },
        { WU_E_CALL_CANCELLED_BY_POLICY, { L"Operation did not complete because the DisableWindowsUpdateAccess policy was set", L"Policy", false } },

        // Self-update errors
        { WU_E_SELFUPDATE_IN_PROGRESS, { L"The operation could not be performed because the Windows Update Agent is self-updating", L"State", true } },

        // Update errors
        { WU_E_INVALID_UPDATE, { L"An update contains invalid metadata", L"Data", false } },
        { WU_E_INVALID_UPDATE_TYPE, { L"The type of update is invalid", L"Data", false } },

        // Network errors
        { WU_E_NO_CONNECTION, { L"Operation did not complete because the network connection was unavailable", L"Network", true } },
        { WU_E_PT_WINHTTP_NAME_NOT_RESOLVED, { L"The proxy server or target server name cannot be resolved", L"Network", true } },

        // User interaction errors
        { WU_E_NO_INTERACTIVE_USER, { L"Operation did not complete because there is no logged-on interactive user", L"User", true } },

        // Timeout errors
        { WU_E_TIME_OUT, { L"Operation did not complete because it timed out", L"Timeout", true } },

        // Bulk operation errors
        { WU_E_ALL_UPDATES_FAILED, { L"Operation failed for all the updates", L"Installation", false } },

        // License errors
        { WU_E_EULAS_DECLINED, { L"The license terms for all updates were declined", L"License", false } },
        { WU_E_EULA_UNAVAILABLE, { L"License terms could not be downloaded", L"License", true } },
        { WU_E_INVALID_PRODUCT_LICENSE, { L"Search may have missed some updates because there is an unlicensed application on the system", L"License", false } },

        // Update availability
        { WU_E_NO_UPDATE, { L"There are no updates", L"Data", true } },

        // URL errors
        { WU_E_URL_TOO_LONG, { L"The URL exceeded the maximum length", L"Data", false } },
        { WU_E_BAD_FILE_URL, { L"The URL does not point to a file", L"Data", false } },

        // Uninstall errors
        { WU_E_UNINSTALL_NOT_ALLOWED, { L"The update could not be uninstalled because the request did not originate from a WSUS server", L"Installation", false } },

        // Component errors
        { WU_E_MISSING_HANDLER, { L"A component required to detect applicable updates was missing", L"Component", false } },

        // Server errors
        { WU_E_LEGACYSERVER, { L"An operation did not complete because it requires a newer version of server", L"Server", false } },

        // Source errors
        { WU_E_BIN_SOURCE_ABSENT, { L"A delta-compressed update could not be installed because it required the source", L"Installation", false } },
        { WU_E_SOURCE_ABSENT, { L"A full-file update could not be installed because it required the source", L"Installation", false } },

        // Access errors
        { WU_E_WU_DISABLED, { L"Access to an unmanaged server is not allowed", L"Permission", false } },

        // Proxy errors
        { WU_E_INVALID_PROXY_SERVER, { L"The format of the proxy list was invalid", L"Network", false } },

        // File errors
        { WU_E_INVALID_FILE, { L"The file is in the wrong format", L"Data", false } },

        // Criteria errors
        { WU_E_INVALID_CRITERIA, { L"The search criteria string was invalid", L"Data", false } },

        // Download errors
        { WU_E_DOWNLOAD_FAILED, { L"Update failed to download", L"Network", true } },

        // Processing errors
        { WU_E_UPDATE_NOT_PROCESSED, { L"The update was not processed", L"Processing", false } },

        // Operation errors
        { WU_E_INVALID_OPERATION, { L"The object's current state did not allow the operation", L"State", false } },
        { WU_E_NOT_SUPPORTED, { L"The functionality for the operation is not supported", L"Support", false } },

        // Resync errors
        { WU_E_TOO_MANY_RESYNC, { L"Agent is asked by server to resync too many times", L"Server", true } },

        // Server core errors
        { WU_E_NO_SERVER_CORE_SUPPORT, { L"The WUA API method does not run on the server core installation", L"Support", false } },

        // Sysprep errors
        { WU_E_SYSPREP_IN_PROGRESS, { L"Service is not available while sysprep is running", L"State", true } },

        // UI errors
        { WU_E_NO_UI_SUPPORT, { L"No support for the WUA user interface", L"Support", false } },

        // Search scope errors
        { WU_E_UNSUPPORTED_SEARCHSCOPE, { L"A search was attempted with a scope that is not currently supported", L"Data", false } },

        // Notification errors
        { WU_E_INVALID_NOTIFICATION_INFO, { L"The featured update notification info returned by the server is invalid", L"Data", false } },

        // Range errors
        { WU_E_OUTOFRANGE, { L"The data is out of range", L"Data", false } },

        // Setup errors
        { WU_E_SETUP_IN_PROGRESS, { L"WUA operations are not available while operating system setup is running", L"State", true } },

        // Unexpected errors
        { WU_E_UNEXPECTED, { L"An operation failed due to reasons not covered by another error code", L"Unknown", false } },

        // Certificate errors
        { CERT_E_EXPIRED, { L"A required certificate is not within its validity period", L"Certificate", false } }
    };

    std::wstring getErrorMessage(HRESULT hr) {
        auto it = errorMap.find(hr);
        if (it != errorMap.end()) {
            return it->second.message;
        }
        return L"Unknown error";
    }

    std::wstring getErrorCategory(HRESULT hr) {
        auto it = errorMap.find(hr);
        if (it != errorMap.end()) {
            return it->second.category;
        }
        return L"Unknown";
    }

    bool isRecoverableError(HRESULT hr) {
        auto it = errorMap.find(hr);
        if (it != errorMap.end()) {
            return it->second.recoverable;
        }
        return false;
    }

} // namespace ErrorMessages
} // namespace WUpdater