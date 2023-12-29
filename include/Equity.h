#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include "utils.h"
#include "curl/curl.h"
using namespace std;

class Equity
{
protected:
    string symbol;
    map<string, double> prices;
public:
    Equity():symbol("") { prices.clear(); }
    Equity(string symbol_) :symbol(symbol_) { prices.clear(); }
    ~Equity() {}
    string GetSymbol()  { return symbol; }
    vector<string> GetDates();
    map<string, double>  GetPrices() { return prices;};
    map<string, double>  GetPrices(int N, string announce_date, vector<string>& trading_dates);
    map<string, double>  GetPrices(map<string, double>& equity_prices);
    void PopulateDatePrice(string start_date, string end_date, CURL* handle);
};



class Stock: public Equity
{
private:
    static vector<string> groupNames;
    groupType group;
    string earning_announce_date, period_ending;
    double estimated_EPS, reported_EPS, surprise, surprise_pct;
public:
    Stock(): Equity(), earning_announce_date(""), period_ending(""), 
        estimated_EPS(0.0), reported_EPS(0.0), surprise(0.0), surprise_pct(0.0){ }
    Stock(string symbol_, string earning_announce_date_, string period_ending_, 
        double estimated_EPS_, double reported_EPS_, double surprise_, double surprise_pct_) :
        Equity(symbol_), earning_announce_date(earning_announce_date_), period_ending(period_ending_), 
        estimated_EPS(estimated_EPS_), reported_EPS(reported_EPS_), surprise(surprise_), surprise_pct(surprise_pct_){ }
    void SetGroup(groupType group_){ group=group_; }
    string GetGroup() { return groupNames[group]; }
    string GetEarningAnnounceDate(){return earning_announce_date;}
    double GetSurprisePct() const {return surprise_pct;}
    using Equity::GetPrices;
    map<string, double>  GetPrices(int N, vector<string>& trading_dates);
    Vector GetReturns(int N, vector<string>& trading_dates, map<string, double> benchmarkPrice);
    friend ostream& operator<<(ostream& out, Stock& stock);
};

