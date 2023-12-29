#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <iterator>
#include "utils.h"
#include "Equity.h"
#include "EquityGroup.h"

using namespace std;

template<class BI>
BI random_unique(BI begin, BI end, size_t sampleSize = 30);

class SampleIndicator {
private:
    //StockGroup &group;
    int N;
    int sampleSize=30;
    vector<string> tradingDates;
    
    Matrix avgAAR;
    Matrix avgCAAR;
    Matrix stddevAAR;
    Matrix stddevCAAR;

public:
    SampleIndicator(int N_, int sampleSize_, vector<string> tradingDates_)
    : N(N_), sampleSize(sampleSize_), tradingDates(tradingDates_) {}
    
    vector<vector<string>> getRandomSample(const vector<vector<string>>& dividedGroups);
    
    map<string, Vector> calculateAbnormalReturns(StockGroup* group, const vector<string>& sample, map<string, double> benchmarkPrice);
    Vector calculateAverageAbnormalReturns(const vector<string>& sample, const map<string, Vector>& abnormalReturns);
    Vector calculateCumulativeAverageAbnormalReturns(const Vector& AAR);
    void performAnalysis(const vector<vector<string>>& dividedSymbols, const vector<StockGroup*>& stockGroups, int repeatSample, map<string, double> benchmarkPrice);
    
    // Methods to calculate average and standard deviation
    Vector calculateAverage(const vector<Vector>& allResults);
    Vector calculateStandardDeviation(const vector<Vector>& allResults, const Vector& avgResults);

    // Accessors
    Matrix getAverageAAR() const { return avgAAR; }
    Matrix getAverageCAAR() const { return avgCAAR; }
    Matrix getStddevAAR() const { return stddevAAR; }
    Matrix getStddevCAAR() const { return stddevCAAR; }

    ~SampleIndicator(){}
};