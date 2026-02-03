//The legacy constraint
// Simulates an old, thread-unsafe, pointer-heavy dependency you inherited.

#pragma once
#include <vector>
#include <string>
#include <thread>   

struct  LegacyItem
{
    char* id;
    int value;
    int quota_remaining;
};

inline std::vector<LegacyItem*> GetLegacyInventory(){
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate delay
    static std::vector<LegacyItem*> items;

    return items;
}