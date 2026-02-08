#pragma once

#include <string>
#include <wuerror.h>

namespace WUpdater {
namespace ErrorMessages {

    /**
     * @brief Get a human-readable error message for a Windows Update error code
     * @param hr The HRESULT error code from Windows Update API
     * @return A descriptive error message string
     */
    std::wstring getErrorMessage(HRESULT hr);

    /**
     * @brief Get a short error category name
     * @param hr The HRESULT error code
     * @return Short category name (e.g., "Network", "Permission", etc.)
     */
    std::wstring getErrorCategory(HRESULT hr);

    /**
     * @brief Check if the error is recoverable (can retry)
     * @param hr The HRESULT error code
     * @return true if the operation can be retried
     */
    bool isRecoverableError(HRESULT hr);

} // namespace ErrorMessages
} // namespace WUpdater