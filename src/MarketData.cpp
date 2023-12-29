#include <stdio.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include <map>
#include "curl/curl.h"
#include "MarketData.h"
#include "Equity.h"
#include "utils.h"

using namespace std;
const char* cIWB3000SymbolFile = "Russell3000EarningsAnnouncements.csv";


pair<int, int> FindIndexesOfDates(int N, string announce_date, vector<string>& dates)
{
    vector<string>::iterator it = find(dates.begin(), dates.end(), announce_date);
    int ind = 0;
    
    if (it != dates.end())
    {
    	ind = distance(dates.begin(), it);
    }
    else
	{
		for(int i = 0; i < (int) dates.size(); i++)
		{
			ind = i;
			if(dates[i] > announce_date) break;
		}
	}
	
    int ind_start = (ind-N >= 0) ? (ind-N) : 0;
    int ind_last = (ind + N < (int) dates.size()) ? (ind + N) : (int) dates.size()-1;
    // cout << ind_start << " " << ind  << " " << ind_last << endl;
    // cout << dates[ind_start] << " " << announce_date << " " << dates[ind_last] ;
    return pair<int, int>(ind_start, ind_last);
}


void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}


int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}


int PullMarketData(const std::string& url_request, void* data, CURL* handle)
{

	// declaration of an object CURL 
	// CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	//curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	// if everything's all right with the easy handle... 
	if (handle)
	{
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

			//adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) data);
			result = curl_easy_perform(handle);

			// check for errors 
			if (result != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				curl_easy_cleanup(handle);
				return -1;
			}

// 			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
// 			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// perform, then store the expected code in result
// 			result = curl_easy_perform(handle);

// 			if (result != CURLE_OK)
// 			{
// 				// if errors have occured, tell us what is wrong with result
// 				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
// 				return 1;
// 			}
			
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return -1;
	}

	// cleanup since you've used curl_easy_init  
	//curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	// curl_global_cleanup();
	
    
    return 0;
}


void PopulateStockMap(map<string, Stock>* stock_map, string start_date, string end_date, int n_stock)
{
	ifstream fin;
	fin.open(cIWB3000SymbolFile, ios::in);
	string line, symbol, date, period_ending, estimate, reported, surprise, surprise_pct;
	getline(fin, line);
	int i = 0;
	while (!fin.eof() && i < n_stock) // i: Number of data for test
	{
		getline(fin, line);
		if (line.find("-") == std::string::npos)
			continue;
		stringstream sin(line);
		getline(sin, symbol, ',');
		getline(sin, date, ',');
		getline(sin, period_ending, ',');
		getline(sin, estimate, ',');
		getline(sin, reported, ',');
		getline(sin, surprise, ',');
		getline(sin, surprise_pct);
		
		date = ConvertDate(date);
		(*stock_map)[symbol] = Stock(symbol, date, period_ending, stod(estimate), stod(reported), stod(surprise), stod(surprise_pct));
		i++;
	}
	pair<size_t, size_t> ind_pair;
	for(map<string, Stock>::iterator it=stock_map->begin(); it!=stock_map->end(); it++)
	{
		cout << it->second.GetSymbol() << endl;
        //it->second.PopulateDatePrice(start_date, end_date);
	}
}

