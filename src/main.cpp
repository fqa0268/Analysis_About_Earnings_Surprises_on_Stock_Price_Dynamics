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
#include <chrono>
#include <thread>
#include "curl/curl.h"
#include "MarketData.h"
#include "Equity.h"
#include "EquityGroup.h"
#include "Bootstrap.h"


using namespace std;


int main(void)
{
	string start_date = "2023-01-01", end_date = "2023-11-30";
	
	curl_global_init(CURL_GLOBAL_ALL);
    CURL* handle = curl_easy_init();
    if (!handle) {
        fprintf(stderr, "Curl init failed!\n");
        return -1;
    }
	Equity IWV("IWV");
	IWV.PopulateDatePrice(start_date, end_date, handle);
	vector<string> trading_dates = IWV.GetDates();
	curl_easy_cleanup(handle);
    curl_global_cleanup();
	
	int N = 60;
// 	int repeatSample = 2; // Number of iterations for the analysis
	int repeatSample = 40; // Number of iterations for the analysis
    int sampleSize = 30; 
	
	
	// initialize group and divided into 3 subgroups based on surprise percentage
	string fileName = "Russell3000EarningsAnnouncements.csv";
	EquityGroup group(fileName);
	group.populateStockInfoMap();
	group.sortBySurprisePct();
	
	// initialize 3 subgroups: beat, meet, and miss
	vector<vector<string>> dividedSymbols = group.getStockGroups();
	
	StockGroup beatGroup(BEAT, dividedSymbols[BEAT], group);
	StockGroup meetGroup(MEET, dividedSymbols[MEET], group);
	StockGroup missGroup(MISS, dividedSymbols[MISS], group);
	
	auto start = chrono::high_resolution_clock::now();
	cout << "Start to retrieve data using multithreading" << endl;
	// utilize multithreading to retrieve data
	curl_global_init(CURL_GLOBAL_ALL);
    CURLM* multi_handle = curl_multi_init();
    vector<thread> threads;
	threads.emplace_back(&StockGroup::populatePrices, ref(beatGroup), start_date, end_date, multi_handle);
	threads.emplace_back(&StockGroup::populatePrices, ref(meetGroup), start_date, end_date, multi_handle);
	threads.emplace_back(&StockGroup::populatePrices, ref(missGroup), start_date, end_date, multi_handle);
    int still_running;
    do {
        curl_multi_perform(multi_handle, &still_running);
    } while (still_running);
    
    for (auto& t: threads) {
        t.join();
    }
    //cout << "StockGroup::populateStockGroupsPrices() complete" << endl;
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();
	
	auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;
	vector<StockGroup*> stockGroups = {&beatGroup, &meetGroup, &missGroup};
	SampleIndicator sampleIndicator(N, sampleSize, trading_dates);
    
    
    //Menu session
    bool running = true;
    bool isPopulated = false;
    string soption, sgroup, ticker;
    
    while (running) {
        cout << endl;   
        cout << "-------------------------------------MENU-------------------------------------" << endl << endl;
        cout << "1. Enter N to retrieve 2N+1 days of historical price data for all stocks" << endl;
        cout << "2. Pull info of one stock" << endl;
        cout << "3. Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
        cout << "4. Show the gnuplot graph with CAAR for all 3 groups" << endl;
        cout << "5. Exit" << endl << endl;
        cout << "Please Enter Number 1-5: ";
        cin >> soption;
        getchar();
        cout << endl;
        cout << "-----------------------------------FUNCTION-----------------------------------" << endl << endl;
        
        
        while ((soption.at(0) > '5' || soption.at(0) <= '0')) {
            cout << "------------------------------------REMINDER----------------------------------" << endl << endl;
            cout << "Invalid option. Please input a number from 1 to 5" << endl;
            cout << "1. Enter N to retrieve 2N+1 days of historical price data for all stocks" << endl;
            cout << "2. Pull info of one stock]" << endl;
            cout << "3. Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
            cout << "4. Show the gnuplot graph with CAAR for all 3 groups" << endl;
            cout << "5. Exit" << endl << endl;
            cout << "Please Enter Number 1-5: ";
            cin >> soption;
            getchar();
            cout << endl;
            cout << "-----------------------------------FUNCTION-----------------------------------" << endl << endl;
        }

		int option = stoi(soption);
		switch (option)
		{

            case 1: {
                cout << "[Enter N to retrieve 2N+1 days of historical price data for all stocks]" << endl << endl;
                int newN;
                cout << "Enter N (N >= 60 & N <= 90): ";
                cin >> newN;
                while (newN < 60 || newN > 90) {
                    cout << "Invalid N. N must be >= 60 and <= 90. Please re-enter: ";
                    cin >> newN;
                }
            
                
                N = newN;
                vector<vector<string>> tmpDividedSymbols = dividedSymbols;
            	beatGroup.cleanSymbols(trading_dates, N, tmpDividedSymbols);
				meetGroup.cleanSymbols(trading_dates, N, tmpDividedSymbols);
				missGroup.cleanSymbols(trading_dates, N, tmpDividedSymbols);
	
    			// Create SampleIndicator instance
    			sampleIndicator = SampleIndicator(N, sampleSize, trading_dates);
    			sampleIndicator.performAnalysis(tmpDividedSymbols, stockGroups, repeatSample, IWV.GetPrices());

    			// Retrieve and display results
    			Matrix avgAAR = sampleIndicator.getAverageAAR();
    			Matrix avgCAAR = sampleIndicator.getAverageCAAR();
    			Matrix stdAAR = sampleIndicator.getStddevAAR();
    			Matrix stdCAAR = sampleIndicator.getStddevCAAR();
    			
				isPopulated = true;	
                
                cout << "N value reset to " << N << " and data reloaded." << endl;
                break;
            }

            
            case 2: {
                cout << "[Pull info of one stock]" << endl;
                cout << "Please enter stock ticker (enter '0' to go back): ";
                cin >> ticker;
                getchar();
            
                if (ticker == "0") {
                    break; 
                }
            
                
                try {
                    
            		Stock stock;
            		if (beatGroup.isSymbolExist(ticker)) {
            			stock = beatGroup.getStockInfo(ticker);
            		} else if (meetGroup.isSymbolExist(ticker)) {
            			stock = meetGroup.getStockInfo(ticker);
            		} else if (missGroup.isSymbolExist(ticker)) {
            			stock = missGroup.getStockInfo(ticker);
            		}
            		
                    
                    cout << stock << endl;
                    
                    auto prices = stock.GetPrices(N, trading_dates);
                    
                    
                    if(prices.size()>0)
                    {
                    	cout << "Prices for " << N << " days around the announcement date:" << endl;
                    	cout << left << setw(15) << "Date" << setw(15) << "Daily Prices" << setw(15) << "Cumulative Daily Returns" << endl;
                    	double cum_log_return = 0;
                        double current_price=1;
                        bool first_price = true;
                    	for (const auto& kv : prices) 
                    	{
                    	   if(first_price)
                    	   {
                    	        cout << left << setw(15) << kv.first << setw(15) << kv.second << setw(15) <<  0.0 << endl;
                    	        first_price = false;
                    	   }
                    	   else
                    	   {
                        	   cum_log_return += log(kv.second/current_price);
                        	   cout << left << setw(15) << kv.first << setw(15) << kv.second << setw(15) <<  exp(cum_log_return) -1  << endl;
                    	   }
                    	   current_price = kv.second;
                    	}
                    }
            
                } catch (const std::out_of_range& e) {
                    cout << "Stock ticker " << ticker << " not found." << endl;
                }
                break;
            }


            case 3: {
                if (!isPopulated) {
                    cout << "Please run option 1 first before checking AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
                    break;
                }
                cout << "[Show AAR, AAR-STD, CAAR and CAAR-STD for one group]" << endl << endl;
                cout << "Please enter the group selection (print '0' to get back): " << endl
                    << "1. Beat group" << endl
                    << "2. Meet group" << endl
                    << "3. Miss group" << endl
                    << endl;
            
                cout << "Please Enter Number 1-3: ";
                cin >> sgroup;
                getchar();
                cout << endl;
            
                if (sgroup.at(0) == '0') break;
            
                int groupnum = stoi(sgroup) - 1; 
            
                Matrix avgAAR = sampleIndicator.getAverageAAR();
                Matrix avgCAAR = sampleIndicator.getAverageCAAR();
                Matrix stdAAR = sampleIndicator.getStddevAAR();
                Matrix stdCAAR = sampleIndicator.getStddevCAAR();
            
                cout << "Detailed Information for selected group:" << endl << endl;
                cout << left << setw(10) << "Days"
                    << setw(15) << "AAR"
                    << setw(15) << "AAR STD"
                    << setw(15) << "CAAR"
                    << setw(15) << "CAAR STD"
                    << endl << endl;
                cout << fixed << setprecision(6);
                for (int i = 0; i < 2 * N; i++) {
                    cout << left << setw(10) << i - N + 1
                        << setw(15) << avgAAR[groupnum][i] 
                        << setw(15) << stdAAR[groupnum][i]
                        << setw(15) << avgCAAR[groupnum][i]
                        << setw(15) << stdCAAR[groupnum][i]
                        << endl;
                }
            
                break;
            }

            case 4: {
                if (!isPopulated) {
                    cout << "Please run option 1 first before checking the gnuplot graph with CAAR for all 3 groups" << endl;
                    break;
                }
                cout << "[Show the gnuplot graph with CAAR for all 3 groups]" << endl << endl;
            
                
                Matrix avgCAAR = sampleIndicator.getAverageCAAR();
            
                
                plotResults(N, avgCAAR);
            
                break;
            }

            case 5: {
                running = false;
                cout << "[Program Exited]" << endl << endl;
                break;
            }
            
           

		}
    }
	return 0;
}

