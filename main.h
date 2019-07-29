#pragma once
#include <wuapi.h>
#include <iostream>
#include <ATLComTime.h>
#include <wuerror.h>

// Search
struct Search {
	IUpdateSession* iUpdate;
	IUpdateSearcher* searcher;
	ISearchResult* results;
};

// Updates
struct Updates {
	IUpdateCollection* List;
	IUpdate* Item;
	LONG              Size;
	BSTR              Name;
	DATE              retdate;
	OperationResultCode RC;
};

/* Progress
struct Progress {
	LONG updateIndex;
	LONG totalPercent;
	LONG Percent;
	IUpdateDownloadResult* result;
};
*/

// Functions and methods
void PrintUInfo(Updates iu);
