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

// Functions and methods
int printUInfo(Updates upd, IUpdateCollection* ToDownloadList);
void syncDownloadUpdates(Updates upd, IUpdateCollection* ToDownloadList, IUpdateDownloader* iDownloader);
void installUpdates(Updates updates);
void signalHandler(int s);

BSTR getCriteria();
int checkHR(HRESULT hr);
