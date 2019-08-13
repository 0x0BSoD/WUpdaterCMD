#pragma once
#include <wuapi.h>
#include <iostream>
#include <ATLComTime.h>
#include <wuerror.h>

// Search
struct Search {
	IUpdateSession*  iUpdate;
	IUpdateSearcher* searcher;
	ISearchResult*   results;
};

// Updates
struct Updates {
	IUpdateCollection* UpdatesList;
	IUpdate*            Item;
	VARIANT_BOOL        InCache;
	LONG                Size;
	BSTR                Name;
	DATE                retdate;
	OperationResultCode RC;
};

// Parameters
struct ArgParameters {
	std::string CriteriaFP;
	bool QuietMode = false;
};

// Functions and methods
int printUInfo(Updates upd, IUpdateCollection* ToDownloadList);
void syncDownloadUpdates(Updates upd, IUpdateCollection* ToDownloadList, IUpdateDownloader* iDownloader);
void installUpdates(Updates updates);
static void signalHandler(int s);
static void showUsage(char* name);
static int parseArgs(int argc, char* argv[], ArgParameters* params);
static BSTR getCriteria(std::string path);
int checkHR(HRESULT hr);
