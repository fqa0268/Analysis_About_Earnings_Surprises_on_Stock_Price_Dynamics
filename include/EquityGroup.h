#pragma once
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "Equity.h"

using namespace std;

class EquityGroup {
  private:
    string fileName;
  protected:
    vector<string> symbols;
    unordered_map<string, Stock> stockInfoMap;
  public:
    // default constructor
    EquityGroup();
    // parameterized constructor
    EquityGroup(string fileName_):fileName(fileName_) {}
    // destructor
    ~EquityGroup() {}
    void sortBySurprisePct();
    // getter
    Stock getStockInfo(const string& symbol) const { return stockInfoMap.at(symbol);}
    unordered_map<string, Stock> getAllStockInfo()  { return stockInfoMap; }
    map<string, double> getPrices(string symbol, int N, vector<string>& trading_dates) { return stockInfoMap[symbol].GetPrices(N, trading_dates); }
    Vector getARreturns(string symbol, int N, vector<string>& trading_dates, map<string, double> benchmarkPrice) { return stockInfoMap[symbol].GetReturns(N, trading_dates, benchmarkPrice); }
    vector<vector<string>> getStockGroups();
    int getNumberOfStocks() { return symbols.size(); }
    
    // setter
    void populateStockInfoMap();
    // helper
    void print();
};

class StockGroup: public EquityGroup {
  private:
    groupType type;
    //unordered_map<string, Stock> group;
  public:
    // default constructor
    StockGroup();
    // parameterized constructor
    StockGroup(groupType type_, const vector<string>& symbols_, const EquityGroup& equityGroup);
    // destructor
    ~StockGroup() {}
    // getter
    bool isSymbolExist(const string& symbol) const { return stockInfoMap.count(symbol); }
    // setter
    void populatePrices(string start_date, string end_date, CURLM* multi_handle);
    void cleanSymbols(vector<string>& trading_dates, int N, vector<vector<string>>& dividedSymbols);
};