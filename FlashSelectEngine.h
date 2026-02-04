#pragma once
#include "Indexer.h"
#include <vector>
#include <algorithm>
#include <span>

class FlashSelectEngine {
    public:
        explicit FlashSelectEngine(const Indexer& indexer)
            : indexer_(indexer) {}

        // THE HOT PATH
        // Returns top K items matching the query.
        // 1. Tokenize query
        // 2. Retrieve items
        // 3. Filter (Quota > 0)
        // 4. Sort (Value DESC)
        // 5. Select Top K
        
        std::vector<ModernItem> _SelectItems(std::string_view query, int k = 5){
            // TODO: Implement the selection logic here.
            // Hint: Use indexer_.GetItemsForKeyword()
            // Hint: Use std::sort with a lambda for ranking
            std::vector<ModernItem> candidates;
            std::unordered_set<std::string_view> seen_ids;
            
            //Tokenize query by spaces
            size_t start = 0;
            while (start < query.size()){
                size_t end = query.find(' ', start);
                if (end == std::string_view::npos){
                    end = query.size();
                }

                std::string_view token = query.substr(start, end - start);
                if(!token.empty()){
                    const auto& items = indexer_.GetItemsForKeyword(std::string(token));
                    for (const auto& item : items){
                        //Filter: Quota > 0 and avoid duplicates
                        if (item.GetQuotaRemaining() > 0){
                            if (seen_ids.insert(item.GetID()).second){
                                candidates.push_back(item);
                            }
                        }
                    }
                }
                start = end + 1;
            }
            //Sort by Value DESC - sorting only unique candidates
            std::sort(candidates.begin(), candidates.end(), [](const ModernItem& a, const ModernItem& b){
                return a.GetValue() > b.GetValue();
            });
            //Select Top K
            if (candidates.size() > static_cast<size_t>(k)){
                candidates.resize(k);
            }
            return candidates;

        }
    private:
        const Indexer& indexer_;

};