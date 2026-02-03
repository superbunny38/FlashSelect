#pragma once
#include "Indexer.h"
#include <vector>
#include <algorithm>
#include <span>

class FlashSelectEngine {
    public:
        explicit FlashSelectEngine(const Indexer& indexer)
            : indexer_(indexer) {}
        
        std::vector<ModernItem> _SelectItems(std::string_view query, int k = 5){
            
        }
    private:
        const Indexer& indexer_;

};