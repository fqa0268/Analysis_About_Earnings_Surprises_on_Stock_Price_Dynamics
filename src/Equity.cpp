#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sstream> 
#include <locale>
#include <iomanip>
#include <fstream>
#include <map>
#include "Equity.h"
#include "MarketData.h"
#include "curl/curl.h"
using namespace std;

vector<string> Stock::groupNames = {"Beat", "Meet", "Miss"};


vector<string> Equity::GetDates()
{
    /* Get dates vector from prices map*/
    vector<string> dates;
    for (const auto& kv : prices) 
    {
        dates.push_back(kv.first);
    }
    return dates; 
}

map<string, double>  Equity::GetPrices(int N, string announce_date, vector<string>& trading_dates)
{ 
    /* Get prices map with size 2*N+1 based on announce_date*/
    pair<size_t, size_t> ind_pair = FindIndexesOfDates(N, announce_date, trading_dates);
    map<string, double> new_prices;
    for(int i=ind_pair.first; i<(int) ind_pair.second+1; i++)
    {
        new_prices[trading_dates[i]] = prices.at(trading_dates[i]);
    }
    return new_prices; 
}


map<string, double> Equity::GetPrices(map<string, double>& equity_prices)
{ 
    /* Get prices map based on anthor equity's trading dates (for trading dates mapping)*/ 
    map<string, double> new_prices;
    for (const auto& kv : equity_prices) 
    {
        new_prices[kv.first] = prices.at(kv.first);
    }
    return new_prices; 
}

    
void Equity::PopulateDatePrice(string start_date, string end_date, CURL* handle)
{
	/* Populate Stock object with dates and adj close data*/
	prices.clear();
	string url_common = "https://eodhistoricaldata.com/api/eod/";
	string api_token = "6568c342858695.65640927";  // You must replace this API token with yours
    
    string read_buffer;
	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;
    
	string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
	PullMarketData(url_request, (void*) &data, handle);
	stringstream sData;
	sData.str(data.memory);
	string sValue, sDate;
	double dValue = 0;
	string line;
// 	cout << symbol << endl;
	while (getline(sData, line)) {
		size_t found = line.find('-');
		if (found != std::string::npos)
		{
			// cout << line << endl;
			sDate = line.substr(0, line.find_first_of(','));
			line.erase(line.find_last_of(','));
			sValue = line.substr(line.find_last_of(',') + 1);
			dValue = strtod(sValue.c_str(), NULL);
// 			cout << sDate << " " << std::fixed << ::setprecision(2) << dValue << endl;
			prices[sDate] = dValue;
		}
	}
	free(data.memory);
	data.size = 0;
}

map<string, double>  Stock::GetPrices(int N, vector<string>& trading_dates)
{
    /* Get dates vector with size 2*N+1 based on the stock object's announce_date*/
    pair<size_t, size_t> ind_pair = FindIndexesOfDates(N, earning_announce_date, trading_dates);
    map<string, double> new_prices;
    for(int i=ind_pair.first; i< (int) ind_pair.second+1; i++)
    {
        if (prices.count(trading_dates[i]) == 0) continue;
        new_prices[trading_dates[i]] = prices.at(trading_dates[i]);
    }
    if ((int)new_prices.size() < 2 * N + 1) {
        cout << symbol << " has missing data. Size of prices data = " << new_prices.size() << endl;
    }
    return new_prices; 
}

Vector Stock::GetReturns(int N, vector<string>& trading_dates, map<string, double> benchmarkPrice)
{
    /* Get dates vector with size 2*N+1 based on the stock object's announce_date*/
    map<string, double> stockPrice = GetPrices(N, trading_dates);
    map<string, double> benchPrice;
    Vector stockReturn;
    Vector benchreturn;
    // pair<size_t, size_t> ind_pair = FindIndexesOfDates(N, earning_announce_date, trading_dates);
    auto l = benchmarkPrice.lower_bound(stockPrice.begin()->first);
    auto r = benchmarkPrice.upper_bound(stockPrice.rbegin()->first);
    for (auto it = l; it != r; it ++) {
        benchPrice[it->first] = benchmarkPrice[it->first];
    }
    
    for (auto it = stockPrice.begin(); it != prev(stockPrice.end()); ++it) {
        double dailyReturn = log(next(it)->second / it->second);
        stockReturn.push_back(dailyReturn);
    }
    
    for (auto it = benchPrice.begin(); it != prev(benchPrice.end()); ++it) {
        double benchReturn = log(next(it)->second / it->second);
        benchreturn.push_back(benchReturn);
    }
    
    if(stockReturn.size()==benchreturn.size()) 
        return stockReturn - benchreturn;
    
}
 
ostream& operator<<(ostream& out, Stock& stock)
{
     /* To output all required info*/
    out << stock.symbol << endl;
    out << "Group: " << stock.GetGroup() << endl;
    out << "Earning Announcement Date: " << stock.earning_announce_date << endl;
    out << "Period Ending: " << stock.period_ending<< endl;
    out << "Estimated EPS:"  << to_string(stock.estimated_EPS) << endl;
    out << "Reported EPS: " <<  to_string(stock.reported_EPS) << endl;
    out << "Surprise: " << to_string(stock.surprise) << endl;
    out << "Surprise %: " << to_string(stock.surprise_pct) << endl;
    out << endl;
    return out;
}


