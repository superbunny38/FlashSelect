//Multi threaded main
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

#include "LegacySystem.h"
#include "ModernAdapter.h"
#include "Indexer.h"   
#include "FlashSelectEngine.h"

//measure time
template<typename Func>
void MeasureTime(const std::string& label, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "[" << label << "] Completed in " << duration.count() << " ms\n";
}

int main(){
    std::cout <<"\n === FlashSelect CONCURRENCY STRESS TEST === \n";

    std::cout << "Setting up legacy inventory...\n";
    std::vector<LegacyItem*> legacy_items = GetLegacyInventory();

    long long initial_total_quota = 0;
    for (auto* item : legacy_items){
        initial_total_quota += item->quota_remaining;
    }

    std::cout << "Initial total quota across all items: " << initial_total_quota << "\n";

    std::cout << "Init Engine...\n";
    RegionMapper region_mapper;
    region_mapper.LoadFromFile("regions.dat");
    region_mapper.PreviewFile("regions.dat");

    Indexer indexer;
    indexer.BuildIndex(legacy_items);
    FlashSelectEngine engine(indexer, region_mapper);

    unsigned int available_cores = std::thread::hardware_concurrency();
    if (available_cores == 0) {
        available_cores = 4; // Default to 4 if unable to detect
    }
    std::cout << "Running concurrent queries with " << available_cores << " threads...\n";
    
    
    const int NUM_THREADS = available_cores;
    const int QUERIES_PER_THREAD = 2500;

    MeasureTime("Concurrent Queries", [&](){
        std::vector<std::thread> threads;
        
        for (int t = 0; t < NUM_THREADS; ++t){
            threads.emplace_back([&engine,t,QUERIES_PER_THREAD](){
                for (int i = 0; i < QUERIES_PER_THREAD; ++i){
                    std::string query = (i % 2 == 0) ? "beauty career" : "excellence happiness";
                    auto results = engine._SelectItems(query, 5);
                    for (auto& item : results){
                        item.DecrementQuota();
                    }
                }
            });
        }

        for (auto& thread : threads){
            if (thread.joinable()){
                thread.join();
            }
            
        }
    });

    cout << "Verifying results...\n";
    long long final_total_quota = 0;
    for (auto* item : legacy_items){
        final_total_quota += item->quota_remaining;
    }

    std::cout << "Final total quota across all items: " << final_total_quota << "\n";
    std::cout << "Total Quota Consumed: " << (initial_total_quota - final_total_quota) << "\n";

    if (final_total_quota < initial_total_quota){
        std::cout << "Quota decremented successfully across threads. Concurrency handling appears correct.\n";
    } else {
        std::cout << "No quota was decremented. There may be an issue with concurrency handling.\n";
    }

    return 0;
}