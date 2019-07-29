#include <wuapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ATLComTime.h>
#include <wuerror.h>
#include "main.h"

using namespace std;

int main(int argc, _TCHAR* argv[])
{
	Search  sr;
	Updates upd;
	HRESULT hr;

	string str_param;
	hr = CoInitialize(NULL);
	
	// Download
	IDownloadResult* IDResult;
	IUpdateDownloadResult* IUDResult;
	IUpdateDownloader *iDownloader;
	IUpdateCollection* ToDownloadList;

	hr = CoCreateInstance(CLSID_UpdateCollection, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateCollection, (LPVOID*)& ToDownloadList);

	// ==========================================================================
	// SEARCH SECTION
	// ==========================================================================
	ifstream param("search_param.txt");
	if (param.is_open())
	{
		wcout << L"Search criteria:" << endl;
		while ( getline(param, str_param) )
		{
			cout << str_param << '\n';
		}
		param.close();
	}
	else 
	{
		wcout << "Unable to open file search_param.txt" << endl;
		return 1;
	}

	const char* cstr = str_param.c_str();
	int wslen = MultiByteToWideChar(CP_ACP, 0, cstr, strlen(cstr), 0, 0);
	BSTR criteria = SysAllocStringLen(0, wslen);
	MultiByteToWideChar(CP_ACP, 0, cstr, strlen(cstr), criteria, wslen);

	hr = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)& sr.iUpdate);
	hr = sr.iUpdate->CreateUpdateSearcher(&sr.searcher);

	wcout << L"Searching for updates ..." << endl;
	hr = sr.searcher->Search(criteria, &sr.results);
	
	switch (hr)
	{
	case S_OK:
		wcout << L"List of applicable items on the machine:" << endl;
		break;
	case WU_E_LEGACYSERVER:
		wcout << L"No server selection enabled" << endl;
		return 0;
	case WU_E_INVALID_CRITERIA:
		wcout << L"Invalid search criteria" << endl;
		return 0;
	}
	SysFreeString(criteria);
	sr.results->get_Updates(&upd.List);
	upd.List->get_Count(&upd.Size);
	// Print updates info
	PrintUInfo(upd);

	// ==========================================================================
	// DOWNLOAD SECTION
	// ==========================================================================
	hr = sr.iUpdate->CreateUpdateDownloader(&iDownloader);
	switch(hr)
	{
	case S_OK:
		wcout << L"Starting download ..." << endl;
		break;
	case E_INVALIDARG:
		wcout << L"A parameter value is invalid" << endl;
		return 0;
	case E_ACCESSDENIED:
		wcout << L"This method cannot be called from a remote computer" << endl;
		return 0;
	}

	// Check cache
	// get_IsDownloaded
	for (LONG i = 0; i < upd.Size; i++)
	{
		long newIndex;
		upd.List->get_Item(i, &upd.Item);
		upd.Item->get_Title(&upd.Name);
		hr = upd.Item->get_IsDownloaded(&upd.InCache);
		switch (hr)
		{
		case S_OK:
			if (upd.InCache) {
				wcout << i + 1 << " - " << upd.Name << " Already downloaded" << endl;

			}
			else {
				hr = ToDownloadList->Add(upd.Item, &newIndex);
				switch (hr)
				{
				case E_POINTER:
					wcout << L" A parameter value is invalid or NULL" << endl;
					return 0;
				case WU_E_NOT_SUPPORTED:
					wcout << L"The collection is read-only" << endl;
					return 0;
				}
			}
			break;
		default:
			wcout << "Som error" << endl;
			return 0;
		}
	}

	// Updates to download
	long tdLen;
	ToDownloadList->get_Count(&tdLen);
	wcout << tdLen << endl;
	if (tdLen > 0) {
		iDownloader->put_Updates(ToDownloadList);

		wcout << L"Downloading updates ..." << endl;
		hr = iDownloader->Download(&IDResult);

		switch (hr)
		{
		case S_OK:
			wcout << L"List of downloaded items on the machine:" << endl;
			for (LONG i = 0; i < tdLen; i++)
			{
				ToDownloadList->get_Item(i, &upd.Item);
				upd.Item->get_Title(&upd.Name);
				IDResult->GetUpdateResult(i, &IUDResult);
				hr = IUDResult->get_ResultCode(&upd.RC);
				switch (upd.RC)
				{
				case 2:
					wcout << i + 1 << " - " << upd.Name << " Successfully downloaded" << endl;
					break;
				default:
					wcout << i + 1 << " - " << upd.Name << " RESULT: " << upd.RC << endl;
					break;
				}
			}
			break;
		case WU_E_PER_MACHINE_UPDATE_ACCESS_DENIED:
			wcout << L"Only administrators can perform this operation on per-computer updates" << endl;
			return 0;
		case WU_E_INVALID_OPERATION:
			wcout << L"The computer cannot access the update site" << endl;
			return 0;
		case WU_E_NO_UPDATE:
			wcout << L"Windows Update Agent (WUA) does not have updates in the collection" << endl;
			return 0;
		case WU_E_NOT_INITIALIZED:
			wcout << L"Windows Update Agent is not initialized" << endl;
			return 0;
		}
	}

	::CoUninitialize();

	return 0;
}


void PrintUInfo(Updates upd) {
	if (upd.Size == 0)
	{
		wcout << L"No updates found" << endl;
	}

	for (LONG i = 0; i < upd.Size; i++)
	{
		upd.List->get_Item(i, &upd.Item);
		upd.Item->get_Title(&upd.Name);
		upd.Item->get_LastDeploymentChangeTime(&upd.retdate);
		COleDateTime odt;
		odt.m_dt = upd.retdate;
		wcout << i + 1 << " - " << upd.Name << "  Release Date " << (LPCTSTR)odt.Format(_T("%A, %B %d, %Y")) << endl;
	}
};
