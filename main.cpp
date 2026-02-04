#include <iostream>
#include <chrono>
#include <vector>
#include <filesystem>
#include "LegacySystem.h"
#include "RegionMapper.h"
#include "Indexer.h"
#include "FlashSelectEngine.h"

//Helper to measure time
template<typename Func>
void Measure(const std::string& label, Func f){
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout<<"\n>>["<<label<<"] "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<"ms\n";
}

int main(){
    cout<<"FlashSelect Engine Starting...\n";
    std::vector<LegacyItem*> legacy_items = GetLegacyInventory();

    cout<<"Loading Region Mapper...\n";
    RegionMapper region_mapper;

    cout<<"Loading Indexer...\n";
    Indexer indexer;
    FlashSelectEngine engine(indexer);
    
    Measure("Boot: RegionMapper", [&](){
        region_mapper.LoadFromFile("regions.dat");
    });
    
    cout<<"\nPreview Region data...\n";
    region_mapper.PreviewFile("regions.dat");

    Measure("Boot: Indexer", [&](){
        indexer.BuildIndex(legacy_items);
    });

    Measure("Runtime: 1000 Queries", [&](){
        for (int i=0; i<1000; i++){
            std::string query = (i%2==0) ? "beauty career" : "excellece happiness";
            auto results = engine._SelectItems(query, 5);
            for (auto& item : results){
                item.DecrementQuota();
            }
            
        }
    });

    auto results = engine._SelectItems("tech news", 5);
    std::cout<<"\nTop 5 Sample Query Results for 'tech news':\n";
    for (const auto& item : results){
        std::cout<<"ID: "<<item.GetID()<<", Value: "<<item.GetValue()<<", Quota Remaining: "<<item.GetQuotaRemaining()<<"\n";
    }

    cout<<"Flash Select: Terminated successfully.\n";
    return 0;
}