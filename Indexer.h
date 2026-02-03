#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "ModernAdapter.h"

class Indexer {
    public:
        //DB
        using IndexTable = std::unordered_map<std::string_view, std::vector<ModernItem>>;

        void BuildIndex(const std::vector<LegacyItem*>& legacy_items) {
            for (auto legacy_item : legacy_items) {
                ModernItem modern_item(legacy_item);
                //parse the id string to find 'keywords' ('_' separated)
                std::string_view id_sv = modern_item.GetID();
                size_t start = 0;
                while (start < id_sv.size()) {
                    size_t end = id_sv.find('_', start);
                    if (end == std::string_view::npos) {
                        end = id_sv.size();
                    }
                    std::string_view keyword = id_sv.substr(start, end - start);
                    index_table_[keyword].push_back(modern_item);
                    start = end + 1;
                }
            }
        }
        const std::vector<ModernItem>& GetItemsForKeyword(const std::string& keyword) const {
            static const std::vector<ModernItem> empty;
            auto it = index_table_.find(keyword);
            if (it != index_table_.end()) {
                return it->second;
            }
            return empty;
        }

    private:
        IndexTable index_table_;
};