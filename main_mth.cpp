//Multi threaded main
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

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

    std::vector<LegacyItem*> legacy_items = GetLegacyInventory();

    

    return 0;
}