// The legacy constraint
// Simulates an old, thread-unsafe, pointer-heavy dependency you inherited.

#pragma once
#include <vector>
#include <cstdio>  // standard C I/O (fopen, fscanf)
#include <cstring> // for strdup
#include <thread>  
#include <iostream>

struct LegacyItem
{
    char* id;
    int value;
    int quota_remaining;
};

// Helper to duplicate strings (cross-platform C-style)
inline char* DuplicateString(const char* str) {
    char* copy = new char[strlen(str) + 1];
    strcpy(copy, str);
    return copy;
}

inline std::vector<LegacyItem*> GetLegacyInventory(){
    // Simulate network/disk latency
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    
    static std::vector<LegacyItem*> items;

    // Only load if we haven't loaded yet (Singleton-ish pattern)
    if (items.empty()) {
        std::cout << "[LegacySystem] Reading inventory from disk...\n";

        FILE* fp = fopen("legacy_inventory.dat", "r");
        if (!fp) {
            std::cerr << "[LegacySystem] Error: Could not open legacy_inventory.dat!\n";
            return items; // Return empty if file missing
        }

        char buffer[256];
        int val, quota;

        // Old C-style parsing: Read string, int, int until EOF
        while (fscanf(fp, "%255s %d %d", buffer, &val, &quota) == 3) {
            LegacyItem* newItem = new LegacyItem();
            newItem->id = DuplicateString(buffer);
            newItem->value = val;
            newItem->quota_remaining = quota;
            items.push_back(newItem);
        }

        fclose(fp);
        std::cout << "[LegacySystem] Loaded " << items.size() << " items.\n";
    }

    return items;
}