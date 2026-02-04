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
    std::cout<<"["<<label<<"]"<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<"ms\n";
}

int main(){
    cout<<"FlashSelect Engine Starting...\n";
    std::vector<LegacyItem*> legacy_items = GetLegacyInventory();

    //

    return 0;
}