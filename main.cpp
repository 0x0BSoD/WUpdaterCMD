#include <wuapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ATLComTime.h>
#include <wuerror.h>
#include "main.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{

	Search  sr;
	Updates updates;
	HRESULT hr;
	hr = CoInitialize(NULL);
	BSTR criteria;
	ArgParameters p;
	int rc;
	char input;

	// Parse arguments
	rc = parseArgs(argc, argv, &p);
	if (rc != 0)
	{
		return -1;
	}


	// Download vars
	IUpdateDownloader* iDownloader;
	IUpdateCollection* ToDownloadList;
	hr = CoCreateInstance(CLSID_UpdateCollection, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateCollection, (LPVOID*)& ToDownloadList);

	// Register signal handler
	signal(SIGINT, signalHandler);


	// ==========================================================================
	// SEARCH SECTION
	// ==========================================================================
	criteria = getCriteria(p.CriteriaFP);
	if (criteria == L"NULL") 
	{
		return -1;
	}

	hr = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)& sr.iUpdate);
	hr = sr.iUpdate->CreateUpdateSearcher(&sr.searcher);
	wcout << endl;
	wcout << L"Searching for updates ..." << endl;
	hr = sr.searcher->Search(criteria, &sr.results);
	if (checkHR(hr) == 0)
	{
		wcout << L"List of applicable items on the machine:" << endl;
		SysFreeString(criteria);
		sr.results->get_Updates(&updates.UpdatesList);
		updates.UpdatesList->get_Count(&updates.Size);

		// Print updates info
		rc = printUInfo(updates, ToDownloadList);
		if (rc != 0) 
		{
			return -1;
		}

		hr = sr.iUpdate->CreateUpdateDownloader(&iDownloader);
		if (checkHR(hr) != 0) 
		{
			return -1;
		}

		// Updates to download
		if (!p.QuietMode) {
			cout << "Download? [y/n]";
			cin >> input;
			if (input != 'y') {
				return 0;
			}
		}
		syncDownloadUpdates(updates, ToDownloadList, iDownloader);
		
		// Updates to install
		if(!p.QuietMode) {
			cout << "Install? [y/n]";
			cin >> input;
			if (input != 'y') {
				return 0;
			}
		}
		installUpdates(updates);
	}
	else
	{
		return -1;
	}
	::CoUninitialize();
	return 0;
}

// ==========================================================================
// Other
// ==========================================================================

// Handle signals, for future usage
static void signalHandler(int s) {
	if (s == 2)
	{
		printf("Caught interrupt\n");
	}
	else 
	{
		printf("Caught signal %d\n", s);
	}
	exit(1);
}

// Show help
static void showUsage(char* name)
{
	cerr << "Usage: " << name << " <option>\n"
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-q,--quiet\t\tRun not asking  agreement\n"
		<< "\t-c,--criteria CRITERIA\tSpecify the path to file with search criteria\n"
		<< "i.e. IsInstalled=0 and Type='Software' and IsHidden=0"
		<< endl;
}

// Arg parser
static int parseArgs(int argc, char* argv[], ArgParameters* params) {
	if (argc < 2) 
	{
		showUsage(argv[0]);
		return -1;
	}

	for (int i = 1; i < argc; ++i) 
	{
		string arg = argv[i];
		if ((arg == "-h") || (arg == "--help")) 
		{
			showUsage(argv[0]);
			return -1;
		}
		else if ((arg == "-c") || (arg == "--criteria")) 
		{
			if (i + 1 < argc) 
			{
				i++;
				params->CriteriaFP = argv[i];
			}
			else {
				cerr << "--criteria option requires one argument." << endl;
				return -1;
			}
		}
		else if ((arg == "-q") || (arg == "--quiet"))
		{
			params->QuietMode = true;
		}
	}
	return 0;
}

// Get search criteria from file
static BSTR getCriteria(string path) {
	string str_param;
	ifstream param(path);

	if (param.is_open())
	{
		wcout << L"Search criteria: ";
		while (getline(param, str_param))
		{
			cout << str_param << '\n';
		}
		param.close();
	}
	else
	{
		wcout << "Unable to open file ";
		cout << path << endl;
		return L"NULL";
	}

	const char* cstr = str_param.c_str();
	int wslen = MultiByteToWideChar(CP_ACP, 0, cstr, strlen(cstr), 0, 0);
	BSTR criteria = SysAllocStringLen(0, wslen);
	MultiByteToWideChar(CP_ACP, 0, cstr, strlen(cstr), criteria, wslen);

	return criteria;
}

// ==========================================================================
// SEARCH SECTION
// ==========================================================================

// Print info about founded updates
int printUInfo(Updates upd, IUpdateCollection* ToDownloadList) {
	HRESULT hr = CoInitialize(NULL);

	if (upd.Size == 0)
	{
		wcout << L"[!] No updates found" << endl;
		return -1;
	}

	for (LONG i = 0; i < upd.Size; i++)
	{
		long newIndex;
		upd.UpdatesList->get_Item(i, &upd.Item);
		upd.Item->get_Title(&upd.Name);
		upd.Item->get_LastDeploymentChangeTime(&upd.retdate);
		COleDateTime odt;
		odt.m_dt = upd.retdate;

		hr = upd.Item->get_IsDownloaded(&upd.InCache);
		switch (hr)
		{
		case S_OK:
			if (upd.InCache) {
				wcout << i + 1 << " - " << upd.Name << "  Release Date " << (LPCTSTR)odt.Format(_T("%A, %B %d, %Y")) << " || Already downloaded" << endl;
			}
			else {
				hr = ToDownloadList->Add(upd.Item, &newIndex);
				wcout << i + 1 << " - " << upd.Name << "  Release Date " << (LPCTSTR)odt.Format(_T("%A, %B %d, %Y")) << " || To download" << endl;
				switch (hr)
				{
				case E_POINTER:
					wcout << L"[!] A parameter value is invalid or NULL" << endl;
					return -1;
				case WU_E_NOT_SUPPORTED:
					wcout << L"[!] The collection is read-only" << endl;
					return -1;
				}
			}
			break;
		default:
			wcout << "[!] Some error" << endl;
			return -1;
		}
	}
	return 0;
};


// ==========================================================================
// DOWNLOAD SECTION
// ==========================================================================
void syncDownloadUpdates(Updates updates, IUpdateCollection* ToDownloadList, IUpdateDownloader* iDownloader) {
	long tdLen;
	IUpdateDownloadResult* IUDResult;
	IDownloadResult* IDResult;
	HRESULT hr = CoInitialize(NULL);

	ToDownloadList->get_Count(&tdLen);
	if (tdLen > 0) 
	{
		iDownloader->put_Updates(ToDownloadList);

		wcout << endl;
		wcout << L"Downloading updates ..." << endl;
		hr = iDownloader->Download(&IDResult);
		if (checkHR(hr) == 0) 
		{
			wcout << L"List of downloaded items on the machine:" << endl;
			for (LONG i = 0; i < tdLen; i++)
			{
				ToDownloadList->get_Item(i, &updates.Item);
				updates.Item->get_Title(&updates.Name);
				IDResult->GetUpdateResult(i, &IUDResult);
				hr = IUDResult->get_ResultCode(&updates.RC);
				switch (updates.RC)
				{
				case 2:
					wcout << i + 1 << " - " << updates.Name << " Successfully downloaded" << endl;
					break;
				default:
					wcout << i + 1 << " - " << updates.Name << " RESULT: " << updates.RC << endl;
					break;
				}
			}
		}
		else
		{
			return;
		}
	}
}

// ==========================================================================
// INSTALL SECTION
// ==========================================================================
void installUpdates(Updates updates) {
	HRESULT hr = CoInitialize(NULL);
	IUpdateInstaller* iUInstaller;
	IInstallationResult* iIResult;
	IUpdateInstallationResult* iUIResult;
	long lenght;

	hr = CoCreateInstance(CLSID_UpdateInstaller, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateInstaller, (LPVOID*)& iUInstaller);
	if (checkHR(hr) != 0)
	{
		return;
	}
	iUInstaller->put_Updates(updates.UpdatesList);

	wcout << endl;
	wcout << "Installation ..." << endl;
	hr = iUInstaller->Install(&iIResult);

	updates.UpdatesList->get_Count(&lenght);
	if (checkHR(hr) == 0)
	{
		wcout << L"List of downloaded items on the machine:" << endl;
		for (LONG i = 0; i < lenght; i++)
		{
			updates.UpdatesList->get_Item(i, &updates.Item);
			updates.Item->get_Title(&updates.Name);
			iIResult->GetUpdateResult(i, &iUIResult);
			hr = iUIResult->get_ResultCode(&updates.RC);
			switch (updates.RC)
			{
			case 0:
				wcout << i + 1 << " - " << updates.Name << " The operation is not started" << endl;
				break;
			case 1:
				wcout << i + 1 << " - " << updates.Name << " The operation is in progress" << endl;
				break;
			case 2:
				wcout << i + 1 << " - " << updates.Name << " Successfully installed" << endl;
				break;
			case 3:
				wcout << i + 1 << " - " << updates.Name << " The operation is complete, but one or more errors occurred during the operation. The results might be incomplete" << endl;
				break;
			case 4:
				wcout << i + 1 << " - " << updates.Name << " The operation failed to complete" << endl;
				break;
			case 5:
				wcout << i + 1 << " - " << updates.Name << " The operation is canceled" << endl;
				break;
			default:
				wcout << i + 1 << " - " << updates.Name << " RESULT: " << updates.RC << endl;
				break;
			}
			if (updates.RC != 2) 
			{
				checkHR(hr);
			}
		}
	}
	else
	{
		return;
	}
}

// ==========================================================================
// ERROR CODES
// ==========================================================================

int checkHR(HRESULT hr) {

	switch (hr)
	{
	case S_OK:
		return 0;
	case WU_E_NO_SERVICE:
		wcout << L"[!] WUA was unable to provide the service" << endl;
		return -1;
	case WU_E_MAX_CAPACITY_REACHED:
		wcout << L"[!] The maximum capacity of the service was exceeded" << endl;
		return -1;
	case WU_E_UNKNOWN_ID:
		wcout << L"[!] WUA cannot find an ID" << endl;
		return -1;
	case WU_E_NOT_INITIALIZED:
		wcout << L"[!] The object could not be initialized" << endl;
		return -1;
	case WU_E_RANGEOVERLAP:
		wcout << L"[!] The update handler requested a byte range overlapping a previously requested range" << endl;
		return -1;
	case WU_E_TOOMANYRANGES:
		wcout << L"[!] The requested number of byte ranges exceeds the maximum number (2³¹ - 1)" << endl;
		return -1;
	case WU_E_INVALIDINDEX:
		wcout << L"[!] The index to a collection was invalid" << endl;
		return -1;
	case WU_E_ITEMNOTFOUND:
		wcout << L"[!] The key for the item queried could not be found" << endl;
		return -1;
	case WU_E_OPERATIONINPROGRESS:
		wcout << L"[!] Another conflicting operation was in progress. Some operations such as installation cannot be performed twice simultaneously" << endl;
		return -1;
	case WU_E_COULDNOTCANCEL:
		wcout << L"[!] Cancellation of the operation was not allowed" << endl;
		return -1;
	case WU_E_CALL_CANCELLED:
		wcout << L"[!] Operation was cancelled" << endl;
		return -1;
	case WU_E_NOOP:
		wcout << L"[!] No operation was required" << endl;
		return -1;
	case WU_E_XML_MISSINGDATA:
		wcout << L"[!] WUA could not find required information in the update's XML data" << endl;
		return -1;
	case WU_E_XML_INVALID:
		wcout << L"[!] WUA found invalid information in the update's XML data" << endl;
		return -1;
	case WU_E_CYCLE_DETECTED:
		wcout << L"[!] Circular update relationships were detected in the metadata" << endl;
		return -1;
	case WU_E_TOO_DEEP_RELATION:
		wcout << L"[!] Update relationships too deep to evaluate were evaluated" << endl;
		return -1;
	case WU_E_INVALID_RELATIONSHIP:
		wcout << L"[!] An invalid update relationship was detected" << endl;
		return -1;
	case WU_E_REG_VALUE_INVALID:
		wcout << L"[!] An invalid registry value was read" << endl;
		return -1;
	case WU_E_DUPLICATE_ITEM:
		wcout << L"[!] Operation tried to add a duplicate item to a list" << endl;
		return -1;
	case WU_E_INVALID_INSTALL_REQUESTED:
		wcout << L"[!] Updates that are requested for install are not installable by the caller" << endl;
		return -1;
	case WU_E_INSTALL_NOT_ALLOWED:
		wcout << L"[!] Operation tried to install while another installation was in progress or the system was pending a mandatory restart" << endl;
		return -1;
	case WU_E_NOT_APPLICABLE:
		wcout << L"[!] Operation was not performed because there are no applicable updates" << endl;
		return -1;
	case WU_E_NO_USERTOKEN:
		wcout << L"[!] Operation failed because a required user token is missing" << endl;
		return -1;
	case WU_E_EXCLUSIVE_INSTALL_CONFLICT:
		wcout << L"[!] An exclusive update can't be installed with other updates at the same time" << endl;
		return -1;
	case WU_E_POLICY_NOT_SET:
		wcout << L"[!] A policy value was not set" << endl;
		return -1;
	case WU_E_SELFUPDATE_IN_PROGRESS:
		wcout << L"[!] The operation could not be performed because the Windows Update Agent is self-updating" << endl;
		return -1;
	case WU_E_INVALID_UPDATE:
		wcout << L"[!] An update contains invalid metadata" << endl;
		return -1;
	case WU_E_SERVICE_STOP:
		wcout << L"[!] Operation did not complete because the service or system was being shut down" << endl;
		return -1;
	case WU_E_NO_CONNECTION:
		wcout << L"[!] Operation did not complete because the network connection was unavailable" << endl;
		return -1;
	case WU_E_NO_INTERACTIVE_USER:
		wcout << L"[!] Operation did not complete because there is no logged-on interactive user" << endl;
		return -1;
	case WU_E_TIME_OUT:
		wcout << L"[!] Operation did not complete because it timed out" << endl;
		return -1;
	case WU_E_ALL_UPDATES_FAILED:
		wcout << L"[!] Operation failed for all the updates" << endl;
		return -1;
	case WU_E_EULAS_DECLINED:
		wcout << L"[!] The license terms for all updates were declined" << endl;
		return -1;
	case WU_E_NO_UPDATE:
		wcout << L"[!] There are no updates" << endl;
		return -1;
	case WU_E_USER_ACCESS_DISABLED:
		wcout << L"[!] Group Policy settings prevented access to Windows Update" << endl;
		return -1;
	case WU_E_INVALID_UPDATE_TYPE:
		wcout << L"[!] The type of update is invalid" << endl;
		return -1;
	case WU_E_URL_TOO_LONG:
		wcout << L"[!] The URL exceeded the maximum length" << endl;
		return -1;
	case WU_E_UNINSTALL_NOT_ALLOWED:
		wcout << L"[!] The update could not be uninstalled because the request did not originate from a Windows Server Update Services (WSUS) server" << endl;
		return -1;
	case  WU_E_INVALID_PRODUCT_LICENSE:
		wcout << L"[!] Search may have missed some updates before there is an unlicensed application on the system" << endl;
		return -1;
	case WU_E_MISSING_HANDLER:
		wcout << L"[!] A component required to detect applicable updates was missing" << endl;
		return -1;
	case WU_E_LEGACYSERVER:
		wcout << L"[!] An operation did not complete because it requires a newer version of server" << endl;
		return -1;
	case WU_E_BIN_SOURCE_ABSENT:
		wcout << L"[!] A delta-compressed update could not be installed because it required the source" << endl;
		return -1;
	case WU_E_SOURCE_ABSENT:
		wcout << L"[!] A full-file update could not be installed because it required the source" << endl;
		return -1;
	case WU_E_WU_DISABLED:
		wcout << L"[!] Access to an unmanaged server is not allowed" << endl;
		return -1;
	case WU_E_CALL_CANCELLED_BY_POLICY:
		wcout << L"[!] Operation did not complete because the DisableWindowsUpdateAccess policy was set in the registry" << endl;
		return -1;
	case WU_E_INVALID_PROXY_SERVER:
		wcout << L"[!] The format of the proxy list was invalid" << endl;
		return -1;
	case WU_E_INVALID_FILE:
		wcout << L"[!] The file is in the wrong format" << endl;
		return -1;
	case WU_E_INVALID_CRITERIA:
		wcout << L"[!] The search criteria string was invalid" << endl;
		return -1;
	case WU_E_EULA_UNAVAILABLE:
		wcout << L"[!] License terms could not be downloaded" << endl;
		return -1;
	case WU_E_DOWNLOAD_FAILED:
		wcout << L"[!] Update failed to download" << endl;
		return -1;
	case WU_E_UPDATE_NOT_PROCESSED:
		wcout << L"[!] The update was not processed" << endl;
		return -1;
	case WU_E_INVALID_OPERATION:
		wcout << L"[!] The object's current state did not allow the operation" << endl;
		return -1;
	case WU_E_NOT_SUPPORTED:
		wcout << L"[!] The functionality for the operation is not supported" << endl;
		return -1;
	case WU_E_TOO_MANY_RESYNC:
		wcout << L"[!] Agent is asked by server to resync too many times" << endl;
		return -1;
	case WU_E_NO_SERVER_CORE_SUPPORT:
		wcout << L"[!] The WUA API method does not run on the server core installation" << endl;
		return -1;
	case WU_E_SYSPREP_IN_PROGRESS:
		wcout << L"[!] Service is not available while sysprep is running" << endl;
		return -1;
	case WU_E_UNKNOWN_SERVICE:
		wcout << L"[!] The update service is no longer registered with automatic updates" << endl;
		return -1;
	case WU_E_NO_UI_SUPPORT:
		wcout << L"[!] No support for the WUA user interface" << endl;
		return -1;
	case WU_E_PER_MACHINE_UPDATE_ACCESS_DENIED:
		wcout << L"[!] Only administrators can perform this operation on per-computer updates" << endl;
		return -1;
	case WU_E_UNSUPPORTED_SEARCHSCOPE:
		wcout << L"[!] A search was attempted with a scope that is not currently supported for this type of search" << endl;
		return -1;
	case WU_E_BAD_FILE_URL:
		wcout << L"[!] The URL does not point to a file" << endl;
		return -1;
		//case WU_E_NOTSUPPORTED:
		//	wcout << L"[!] The operation requested is not supported" << endl;
		//	return -1;
	case WU_E_INVALID_NOTIFICATION_INFO:
		wcout << L"[!] The featured update notification info returned by the server is invalid" << endl;
		return -1;
	case WU_E_OUTOFRANGE:
		wcout << L"[!] The data is out of range" << endl;
		return -1;
	case WU_E_SETUP_IN_PROGRESS:
		wcout << L"[!] WUA operations are not available while operating system setup is running" << endl;
		return -1;
	case WU_E_UNEXPECTED:
		wcout << L"[!] An operation failed due to reasons not covered by another error code" << endl;
		return -1;
	default:
		wcout << "[!] Some error" << endl;
		return -1;
	}

	return 0;
}
