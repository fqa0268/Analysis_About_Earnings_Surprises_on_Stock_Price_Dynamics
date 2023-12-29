#include "Bootstrap.h"
#include "EquityGroup.h"
#include "utils.h"
#include <cmath>

using namespace std;

//get 30 sample stock 
template<class BI>
BI random_unique(BI begin, BI end, int sampleSize = 30) {
    int left = distance(begin, end);
    random_device rd;
    mt19937 gen(rd());

    // Check if the samplesize exceeds stockGroup size
    sampleSize = min(sampleSize, left);
    
    while (sampleSize-- && left > 0) {
        auto r = next(begin, uniform_int_distribution<int>(0, --left)(gen));
        swap(*begin++, *r);
    }
    return begin;
}


// use
// random_unique(stockGroup.begin(), stockGroup.end(), sampleSize);
// vector<string> sampleGroup(stockGroup.begin(), stockGroup.begin() + sampleSize);


vector<vector<string>> SampleIndicator::getRandomSample(const vector<vector<string>>& dividedGroups) {
    vector<vector<string>> samples;
    for (auto &groupVec : {dividedGroups[BEAT], dividedGroups[MEET], dividedGroups[MISS]}) {
        vector<string> groupSymbols;
        for (const auto &symbol : groupVec) {
            groupSymbols.push_back(symbol);
        }
        
        random_unique(groupSymbols.begin(), groupSymbols.end(), sampleSize);
        vector<string> sample(groupSymbols.begin(), groupSymbols.begin() + min(sampleSize, static_cast<int>(groupSymbols.size())));
        samples.push_back(sample);
    }
    
    return samples;
}
map<string, Vector> SampleIndicator::calculateAbnormalReturns(StockGroup* group, const vector<string>& sample,  map<string, double> benchmarkPrice) {
    map<string, Vector> sampleAbnormalReturns;
    for (const auto& stockSymbol : sample) {
        cout<<stockSymbol<<endl;
        auto ARreturns = group->getARreturns(stockSymbol, N, tradingDates, benchmarkPrice);
        // cout<<ARreturns.size()<<endl;
        sampleAbnormalReturns[stockSymbol] = ARreturns;
    }
    return sampleAbnormalReturns;
}

Vector SampleIndicator::calculateAverageAbnormalReturns(const vector<string>& sample, const map<string, Vector>& abnormalReturns) {
    Vector AAR(2 * N, 0.0);
    for (int day = 0; day < 2 * N; ++day) {
        double sumAbnormalReturns = 0.0;
        for (const auto& stockSymbol : sample) {
            sumAbnormalReturns += abnormalReturns.at(stockSymbol)[day];
        }
        AAR[day] = sumAbnormalReturns / sample.size();
    }
    return AAR;
}

Vector SampleIndicator::calculateCumulativeAverageAbnormalReturns(const Vector& AAR) {
    Vector CAAR(2 * N, 0.0);
    CAAR[0] = AAR[0];
    for (size_t day = 1; day < CAAR.size(); ++day) {
        CAAR[day] += CAAR[day - 1]+ AAR[day];
    }
    return CAAR;
}

Vector SampleIndicator::calculateAverage(const vector<Vector>& allResults) {
    // cout<<allResults.size()<<endl;
    int numSamplings = allResults.size();
    if (numSamplings == 0) {
        return Vector(); // Return an empty vector if no data
    }

    int numDays = allResults[0].size(); // Number of days (2N)
    Vector avgResults(numDays, 0.0);

    for (const auto& result : allResults) {
        for (int i = 0; i < numDays; ++i) {
            avgResults[i] += result[i];
        }
    }

    for (int i = 0; i < numDays; ++i) {
        avgResults[i] /= numSamplings;
        // cout<<avgResults[i]<<endl;
    }
    
    return avgResults;
}

Vector SampleIndicator::calculateStandardDeviation(const vector<Vector>& allResults, const Vector& avgResult) {
    int numSamplings = allResults.size();
    if (numSamplings == 0) {
        return Vector(); // Return an empty vector if no data
    }

    int numDays = allResults[0].size(); // Number of days (2N)
    Vector stddevResults(numDays, 0.0);

    for (const auto& result : allResults) {
        for (int i = 0; i < numDays; ++i) {
            stddevResults[i] += pow(result[i] - avgResult[i], 2);
            // cout<<stddevResults[i]<<endl;
        }
    }

    for (int i = 0; i < numDays; ++i) {
        stddevResults[i] = sqrt(stddevResults[i] / (numSamplings - 1));
        // cout<<stddevResults[i]<<endl;
    }

    return stddevResults;
}

void SampleIndicator::performAnalysis(const vector<vector<string>>& dividedSymbols, const vector<StockGroup*>& stockGroups, int repeatSample, map<string, double> benchmarkPrice) {
    vector<vector<Vector>> allAARs(3, vector<Vector>(repeatSample, Vector(2 * N, 0.0)));
    vector<vector<Vector>> allCAARs(3, vector<Vector>(repeatSample, Vector(2 * N, 0.0)));

    cout << "Starting analysis..." << endl;

    for (int iter = 0; iter < repeatSample; ++iter) {
        cout << "Iteration " << (iter + 1) << "/" << repeatSample << "..." << endl;
        vector<vector<string>> groupSamples = getRandomSample(dividedSymbols); // 3 groups of samples

        for (unsigned int group = 0; group < groupSamples.size(); ++group) {
            cout << "Processing group " << group << "..." << endl;
            const auto& sample = groupSamples[group];

            // Calculate returns and abnormal returns for the current sample group
            auto abnormalReturns = calculateAbnormalReturns(stockGroups[group], sample, benchmarkPrice);
            auto AAR = calculateAverageAbnormalReturns(sample, abnormalReturns);
            auto CAAR = calculateCumulativeAverageAbnormalReturns(AAR);
            // Calculate AAR and CAAR for the current group and iteration
            allAARs[group][iter] = AAR;
            allCAARs[group][iter] = CAAR;
        }

        cout << "Iteration " << (iter + 1) << " completed." << endl;
    }

    cout << "Aggregating results to calculate average and standard deviation..." << endl;
    // Aggregating results for each group
    for (int group = 0; group < 3; ++group) {
        avgAAR.push_back(calculateAverage(allAARs[group]));
        avgCAAR.push_back(calculateAverage(allCAARs[group]));
        stddevAAR.push_back(calculateStandardDeviation(allAARs[group], avgAAR.back()));
        stddevCAAR.push_back(calculateStandardDeviation(allCAARs[group], avgCAAR.back()));
    }

    cout << "Analysis completed successfully." << endl;
}