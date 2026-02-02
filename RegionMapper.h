#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <charconv>

using namespace std;

class RegionMapper {
    public:
        void LoadFromFile(const string& filename){
            //1. Resize dataBuffer to file size
            auto size = filesystem::file_size(filename);
            dataBuffer.resize(size);

            //2. Read the entire file into dataBuffer
            ifstream file(filename, ios::binary);
            file.read(dataBuffer.data(), size);

            //3. Iterate through dataBuffer to parse ID and Name pairs
            const char* ptr = dataBuffer.data();
            const char* end = ptr + size;

            while (ptr < end) {
                int id;
                auto [next, ec] = from_chars(ptr, end, id);
                if (ec != std::errc()) {
                    cerr << "Error parsing ID\n";
                    break;
                }

                ptr = next+1; // Move past the null terminator

                const char* name_start = ptr;
                while (ptr < end && *ptr != '\n' && *ptr != '\r') {
                    ptr++;
                }

                //4. Update lookups (id_to_name_ and name_to_id_)
                string_view name(name_start, static_cast<size_t>(ptr - name_start));

                if (id>= id_to_name_.size()) {
                    id_to_name_.resize(id + 1);
                }
                id_to_name_[id] = name;
                name_to_id_[name] = id;

                //skip newline characters
                while (ptr < end && (*ptr == '\n' || *ptr == '\r')) {
                    ptr++;
                
                }
            }
            
        }

        //O(1) lookup: Returns the name corresponding to the given ID. (Returns a view into the main buffer)
        string_view GetName(int id) const{
            if (id < 0 || id >= id_to_name_.size()) {
                return "Unknown";
            }
            return id_to_name_[id];
        }

        //O(1) lookup: Returns the ID corresponding to the given name. (Returns -1 if not found)
        int GetId(string_view name) const{
            auto it = name_to_id_.find(name);
            if (it != name_to_id_.end()) {
                return it->second;
            }
            return -1; // Not found
        }
    
    private:
        vector<char> dataBuffer;//the Giant Buffer - stores the entire file content (We keep it alive to ensure string_views remain valid)
        vector<string_view> id_to_name_;
        unordered_map<string_view, int> name_to_id_;
};