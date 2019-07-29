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
	
	// Progress
	//IDownloadProgress* iDProgress;
	//IInstallationProgress* iIProgress;

	// Download
	IDownloadResult* IDResult;
	IUpdateDownloadResult* IUDResult;
	IUpdateDownloader *iDownloader;

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

	// Updates to download
	iDownloader->put_Updates(upd.List);

	
	wcout << L"Downloading updates ..." << endl;
	hr = iDownloader->Download(&IDResult);
	wcout << hr << endl;
	switch (hr)
	{
	case S_OK:
		wcout << L"List of downloaded items on the machine:" << endl;
		for (LONG i = 0; i < upd.Size; i++)
		{
			upd.List->get_Item(i, &upd.Item);
			upd.Item->get_Title(&upd.Name);
			IDResult->GetUpdateResult(i, &IUDResult);
			hr = IUDResult->get_ResultCode(&upd.RC);

			wcout << i + 1 << " - " << upd.Name << "  RESULT: " << upd.RC << endl;
		}
		break;
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

	/*
	iDownload->get_Updates(&upd.updateList);
	iDownload->GetProgress(&iDProgress);
	long int done = (long int)100;
	for (LONG i = 0; i < upd.updateSize; i++) {
		bool dFlag = true;
		wcout << "[!] Downloading - " << upd.updateName << endl;
		
		while (dFlag) {
			iDProgress->get_PercentComplete(&progress.totalPercent);
			wcout << "% - " << progress.totalPercent << endl;
		}
		
		// iDProgress->get_CurrentUpdatePercentComplete(&progress.Percent);
		if (&progress.Percent >= &done) {
			iDProgress->GetUpdateResult(i, &progress.result);
			dFlag = false;
		}
	}
	*/

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
