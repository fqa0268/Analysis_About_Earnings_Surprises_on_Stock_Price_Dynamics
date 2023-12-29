#include <stdio.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include <thread>
#include "curl/curl.h"
#include <float.h>
#include "EquityGroup.h"
using namespace std;

EquityGroup::EquityGroup() {
    fileName = "Russell3000EarningsAnnouncements.csv";
}

void EquityGroup::populateStockInfoMap() {
    ifstream fin;
	fin.open(fileName, ios::in);
	// ticker,date,period_ending,estimate,reported,surprise,surprise%
	string line, ticker, date, periodEnding;
	double estimate, reported, surprise, surprisePct;
	getline(fin, line);
	while (getline(fin, line)) {
	    ticker = line.substr(0, line.find_first_of(','));
	    symbols.push_back(ticker);
	    line = line.substr(line.find_first_of(',') + 1);
	    
	    date = line.substr(0, line.find_first_of(','));
	    date = ConvertDate(date);
	    line = line.substr(line.find_first_of(',') + 1);

	    periodEnding = line.substr(0, line.find_first_of(','));
	    line = line.substr(line.find_first_of(',') + 1);
	    
	    estimate = stod(line.substr(0, line.find_first_of(',')));
	    line = line.substr(line.find_first_of(',') + 1);
	    
	    reported = stod(line.substr(0, line.find_first_of(',')));
	    line = line.substr(line.find_first_of(',') + 1);
	    
	    surprise = stod(line.substr(0, line.find_first_of(',')));
	    line = line.substr(line.find_first_of(',') + 1);
	    
	    surprisePct = stod(line.substr(0, line.find_first_of(',')));
	    
	    Stock stock(ticker, date, periodEnding, estimate, reported, surprise, surprisePct);
	    stockInfoMap.insert(make_pair(ticker, stock));
	}
	fin.close();
}

void EquityGroup::sortBySurprisePct() {
    // sort in descending order
    symbols.clear();
    symbols.resize(stockInfoMap.size());
    // map => vector
    vector<pair<string, Stock>> vec(stockInfoMap.begin(), stockInfoMap.end());
    sort(vec.begin(), vec.end(), [](const pair<string, Stock>& p1, const pair<string, Stock>& p2) {
        return p1.second.GetSurprisePct() > p2.second.GetSurprisePct();
    });
    
    size_t idx = 0;
    for (const auto& v: vec) {
        symbols[idx ++] = v.first;
    }
}

vector<vector<string>> EquityGroup::getStockGroups() {
    vector<vector<string>> stockGroups;
    int group_size = symbols.size() / 3;
    
    vector<string> beatEstimateGroup(symbols.begin(), symbols.begin() + group_size);
    vector<string> meetEstimateGroup(symbols.begin() + group_size, symbols.begin() + group_size * 2);
    vector<string> missEstimateGroup(symbols.begin() + group_size * 2, symbols.end());
    
    stockGroups.emplace_back(beatEstimateGroup); // 0
    stockGroups.emplace_back(meetEstimateGroup); // 1
    stockGroups.emplace_back(missEstimateGroup); // 2
    
    return stockGroups;
}

void EquityGroup::print() {
    for (const auto& symbol: symbols) {
        cout << symbol << " " << stockInfoMap[symbol].GetEarningAnnounceDate() << " " << stockInfoMap[symbol].GetSurprisePct() << endl;
    }
}

StockGroup::StockGroup(groupType type_, const vector<string>& symbols_, const EquityGroup& equityGroup) {
    type = type_;
    symbols = symbols_;
    for (const auto& symbol: symbols) {
        stockInfoMap[symbol] = equityGroup.getStockInfo(symbol);
        stockInfoMap[symbol].SetGroup(type);
    }
}

void StockGroup::populatePrices(string start_date, string end_date, CURLM* multi_handle) {
    CURL* easy_handle = curl_easy_init();
    //for (auto& [symbol, stock]: stockInfoMap) {
    for (auto iter = stockInfoMap.begin(); iter != stockInfoMap.end(); iter ++) {
        iter->second.PopulateDatePrice(start_date, end_date, easy_handle);
    }
    curl_multi_add_handle(multi_handle, easy_handle);
}

void StockGroup::cleanSymbols(vector<string>& trading_dates, int N, vector<vector<string>>& dividedSymbols) {
    vector<string> invalidStocks;
    for (auto& [symbol, stock]: stockInfoMap) {
        if ((int)stock.GetPrices(N, trading_dates).size() != 2 * N + 1) {
            invalidStocks.push_back(symbol);
        }
    }
    for (const auto& symbol: invalidStocks) {
        //stockInfoMap.erase(symbol);
        //symbols.erase(find(symbols.begin(), symbols.end(), symbol));
        
        dividedSymbols[type].erase(find(dividedSymbols[type].begin(), dividedSymbols[type].end(), symbol));
    }
}

