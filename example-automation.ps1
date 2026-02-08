# Example automation script for WUpdaterCMD
# This script demonstrates how to use WUpdaterCMD in automated scenarios
# Run this script with Administrator privileges

#Requires -RunAsAdministrator

param(
    [Parameter()]
    [string]$CriteriaFile = "criteria.txt",

    [Parameter()]
    [string]$LogFile = "update-log.txt",

    [Parameter()]
    [switch]$SkipDownload,

    [Parameter()]
    [switch]$SkipInstall,

    [Parameter()]
    [switch]$RebootIfNeeded
)

$ErrorActionPreference = "Stop"

# Configuration
$WUpdaterPath = ".\build\bin\Release\WUpdaterCMD.exe"
$MaxRetries = 3
$RetryDelaySeconds = 30

# Function to write log with timestamp
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO"
    )

    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"

    Write-Host $logMessage
    Add-Content -Path $LogFile -Value $logMessage
}

# Function to check if reboot is pending
function Test-PendingReboot {
    $rebootPending = $false

    # Check CBS RebootPending
    if (Test-Path "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Component Based Servicing\RebootPending") {
        $rebootPending = $true
    }

    # Check Windows Update RebootRequired
    if (Test-Path "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Auto Update\RebootRequired") {
        $rebootPending = $true
    }

    # Check PendingFileRenameOperations
    $fileRenameOps = Get-ItemProperty "HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager" -Name PendingFileRenameOperations -ErrorAction SilentlyContinue
    if ($fileRenameOps) {
        $rebootPending = $true
    }

    return $rebootPending
}

# Function to send email notification (configure your SMTP settings)
function Send-UpdateNotification {
    param(
        [string]$Subject,
        [string]$Body,
        [string]$Priority = "Normal"
    )

    # TODO: Configure your SMTP settings
    $SmtpServer = "smtp.example.com"
    $SmtpPort = 587
    $From = "updates@example.com"
    $To = "admin@example.com"
    $Username = "smtp-user"
    $Password = "smtp-password"

    try {
        # Uncomment and configure for email notifications
        # $SecurePassword = ConvertTo-SecureString $Password -AsPlainText -Force
        # $Credential = New-Object System.Management.Automation.PSCredential($Username, $SecurePassword)
        # Send-MailMessage -SmtpServer $SmtpServer -Port $SmtpPort -From $From -To $To `
        #     -Subject $Subject -Body $Body -Credential $Credential -UseSsl -Priority $Priority

        Write-Log "Email notification: $Subject" "INFO"
    } catch {
        Write-Log "Failed to send email notification: $_" "WARNING"
    }
}

# Main script execution
try {
    Write-Log "=== Windows Update Automation Started ===" "INFO"
    Write-Log "Computer: $env:COMPUTERNAME" "INFO"
    Write-Log "User: $env:USERNAME" "INFO"
    Write-Log "Criteria File: $CriteriaFile" "INFO"

    # Check if running as Administrator
    $isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
    if (-not $isAdmin) {
        Write-Log "This script must be run as Administrator!" "ERROR"
        exit 1
    }

    # Check if WUpdaterCMD exists
    if (-not (Test-Path $WUpdaterPath)) {
        Write-Log "WUpdaterCMD not found at: $WUpdaterPath" "ERROR"
        Write-Log "Please build the project first using build.ps1" "ERROR"
        exit 1
    }

    # Check if criteria file exists
    if (-not (Test-Path $CriteriaFile)) {
        Write-Log "Criteria file not found: $CriteriaFile" "ERROR"
        Write-Log "Creating default criteria file..." "INFO"
        "IsInstalled=0 and Type='Software' and IsHidden=0" | Out-File -FilePath $CriteriaFile -Encoding UTF8
    }

    # Check Windows Update service
    $wuauserv = Get-Service -Name wuauserv -ErrorAction SilentlyContinue
    if ($wuauserv.Status -ne "Running") {
        Write-Log "Windows Update service is not running. Starting..." "WARNING"
        Start-Service -Name wuauserv
        Start-Sleep -Seconds 5
    }

    # Check for pending reboot before starting
    if (Test-PendingReboot) {
        Write-Log "System has pending reboot. Please reboot before applying updates." "WARNING"
        if ($RebootIfNeeded) {
            Write-Log "Initiating system reboot in 60 seconds..." "WARNING"
            Send-UpdateNotification -Subject "[$env:COMPUTERNAME] Reboot Required Before Updates" `
                -Body "System has pending reboot. Rebooting in 60 seconds..." -Priority "High"
            shutdown /r /t 60 /c "Reboot required before applying Windows Updates"
            exit 0
        } else {
            Write-Log "Use -RebootIfNeeded parameter to automatically reboot" "INFO"
            exit 2
        }
    }

    # Execute WUpdaterCMD with retry logic
    $retryCount = 0
    $success = $false

    while ($retryCount -lt $MaxRetries -and -not $success) {
        try {
            if ($retryCount -gt 0) {
                Write-Log "Retry attempt $retryCount of $MaxRetries..." "INFO"
                Start-Sleep -Seconds $RetryDelaySeconds
            }

            Write-Log "Executing WUpdaterCMD..." "INFO"

            # Run WUpdaterCMD in quiet mode
            $process = Start-Process -FilePath $WUpdaterPath `
                -ArgumentList "-c", $CriteriaFile, "--quiet" `
                -Wait -NoNewWindow -PassThru -RedirectStandardOutput "update-output.txt" -RedirectStandardError "update-error.txt"

            # Check exit code
            if ($process.ExitCode -eq 0) {
                Write-Log "WUpdaterCMD completed successfully" "SUCCESS"
                $success = $true

                # Read and log output
                if (Test-Path "update-output.txt") {
                    $output = Get-Content "update-output.txt" -Raw
                    Write-Log "Output: $output" "INFO"
                }

            } else {
                Write-Log "WUpdaterCMD exited with code: $($process.ExitCode)" "ERROR"

                # Read and log error
                if (Test-Path "update-error.txt") {
                    $error = Get-Content "update-error.txt" -Raw
                    Write-Log "Error: $error" "ERROR"
                }

                $retryCount++
            }

        } catch {
            Write-Log "Exception occurred: $_" "ERROR"
            $retryCount++
        }
    }

    if (-not $success) {
        Write-Log "Failed to complete updates after $MaxRetries attempts" "ERROR"
        Send-UpdateNotification -Subject "[$env:COMPUTERNAME] Windows Update Failed" `
            -Body "Failed to complete Windows Updates after $MaxRetries attempts. Check logs: $LogFile" `
            -Priority "High"
        exit 1
    }

    # Check for pending reboot after updates
    if (Test-PendingReboot) {
        Write-Log "Updates installed successfully. System reboot required." "WARNING"
        Send-UpdateNotification -Subject "[$env:COMPUTERNAME] Windows Updates Installed - Reboot Required" `
            -Body "Windows Updates have been installed successfully. System reboot is required." `
            -Priority "High"

        if ($RebootIfNeeded) {
            Write-Log "Initiating system reboot in 5 minutes..." "WARNING"
            shutdown /r /t 300 /c "Reboot required after Windows Updates. Rebooting in 5 minutes..."
        } else {
            Write-Log "Use -RebootIfNeeded parameter to automatically reboot" "INFO"
        }
    } else {
        Write-Log "Updates completed. No reboot required at this time." "SUCCESS"
        Send-UpdateNotification -Subject "[$env:COMPUTERNAME] Windows Updates Completed" `
            -Body "Windows Updates have been installed successfully. No reboot required."
    }

    # Cleanup temporary files
    Remove-Item "update-output.txt" -ErrorAction SilentlyContinue
    Remove-Item "update-error.txt" -ErrorAction SilentlyContinue

    Write-Log "=== Windows Update Automation Completed ===" "INFO"
    exit 0

} catch {
    Write-Log "Fatal error: $_" "ERROR"
    Write-Log "Stack trace: $($_.ScriptStackTrace)" "ERROR"

    Send-UpdateNotification -Subject "[$env:COMPUTERNAME] Windows Update Automation Error" `
        -Body "Fatal error occurred: $_`n`nCheck logs: $LogFile" `
        -Priority "High"

    exit 1
}

# Example usage:
#
# Basic usage (interactive prompts):
#   .\example-automation.ps1
#
# Fully automated with reboot:
#   .\example-automation.ps1 -RebootIfNeeded
#
# Custom criteria file:
#   .\example-automation.ps1 -CriteriaFile "security-only.txt"
#
# Custom log location:
#   .\example-automation.ps1 -LogFile "C:\Logs\updates.log"
#
# Scheduled task example:
#   schtasks /create /tn "Windows Update Automation" /tr "powershell.exe -ExecutionPolicy Bypass -File C:\Scripts\example-automation.ps1 -RebootIfNeeded" /sc weekly /d SUN /st 02:00 /ru SYSTEM
