# Build script for WUpdaterCMD using CMake
# PowerShell version

param(
    [Parameter(Position=0)]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildType = "Release",

    [Parameter()]
    [switch]$Clean,

    [Parameter()]
    [switch]$Verbose
)

$ErrorActionPreference = "Stop"

# Color output functions
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Write-Info($message) {
    Write-ColorOutput Cyan "INFO: $message"
}

function Write-Success($message) {
    Write-ColorOutput Green "SUCCESS: $message"
}

function Write-Error-Custom($message) {
    Write-ColorOutput Red "ERROR: $message"
}

# Print header
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Building WUpdaterCMD" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if CMake is available
try {
    $cmakeVersion = & cmake --version 2>&1 | Select-Object -First 1
    Write-Info "Found CMake: $cmakeVersion"
} catch {
    Write-Error-Custom "CMake not found in PATH!"
    Write-Host "Please install CMake and add it to your PATH."
    Write-Host "Download from: https://cmake.org/download/"
    exit 1
}

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-Info "Cleaning build directory..."
    Remove-Item -Recurse -Force "build"
}

# Create build directory
if (-not (Test-Path "build")) {
    Write-Info "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Navigate to build directory
Push-Location "build"

try {
    # Configure CMake
    Write-Host ""
    Write-Info "Configuring project with CMake..."

    $cmakeArgs = @("..", "-DCMAKE_BUILD_TYPE=$BuildType")

    if ($Verbose) {
        $cmakeArgs += "--debug-output"
    }

    & cmake @cmakeArgs

    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "CMake configuration failed!"
        exit 1
    }

    # Build the project
    Write-Host ""
    Write-Info "Building project..."

    $buildArgs = @("--build", ".", "--config", $BuildType)

    if ($Verbose) {
        $buildArgs += "--verbose"
    }

    & cmake @buildArgs

    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Build failed!"
        exit 1
    }

    # Success message
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Success "Build completed successfully!"
    Write-Host ""

    # Determine executable path
    $exePath = ""
    if ($BuildType -eq "Debug") {
        $exePath = "build\bin\Debug\WUpdaterCMD.exe"
    } else {
        $exePath = "build\bin\$BuildType\WUpdaterCMD.exe"
    }

    # Check if executable exists
    $fullPath = Join-Path $PSScriptRoot $exePath
    if (Test-Path $fullPath) {
        Write-Host "Executable location:" -ForegroundColor Cyan
        Write-Host "  $exePath" -ForegroundColor Yellow

        # Get file size
        $fileSize = (Get-Item $fullPath).Length
        $fileSizeKB = [math]::Round($fileSize / 1KB, 2)
        Write-Host "  Size: $fileSizeKB KB" -ForegroundColor Gray
    }

    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""

} catch {
    Write-Error-Custom "An error occurred during build: $_"
    exit 1
} finally {
    # Return to original directory
    Pop-Location
}

# Additional information
Write-Host "Usage examples:" -ForegroundColor Cyan
Write-Host "  .\$exePath -c criteria.txt" -ForegroundColor Gray
Write-Host "  .\$exePath -c criteria.txt --quiet" -ForegroundColor Gray
Write-Host ""
