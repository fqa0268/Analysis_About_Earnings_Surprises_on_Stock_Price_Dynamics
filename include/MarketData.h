#pragma once
#include <string>
#include <vector>
#include <map>
#include "Equity.h"
#include "curl/curl.h"

using namespace std;

struct MemoryStruct {
	char* memory;
	size_t size;
};

//string ConvertDate(string date);
pair<int, int> FindIndexesOfDates(int N, string announce_date, vector<string>& dates);
void* myrealloc(void* ptr, size_t size);
int write_data2(void* ptr, size_t size, size_t nmemb, void* data);
int PullMarketData(const std::string& url_request, void* data, CURL* handle);
void PopulateStockMap(map<string, Stock>* stock_map, string start_date, string end_date, int n_stock=9999);

