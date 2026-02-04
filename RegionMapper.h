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
            auto size = filesystem::file_size(filename);
            dataBuffer.resize(size);

            ifstream file(filename, ios::binary);
            file.read(dataBuffer.data(), size);

            const char* ptr = dataBuffer.data();
            const char* end = ptr + size;

            while (ptr<end)
            {
                int id;
                auto [next, ec] = from_chars(ptr, end, id);
                if (ec != std::errc()) {
                    cerr << "Error parsing ID\n";
                    break;
                }

                ptr = next+1;//move to the start of the name (skip comma)

                const char* comma_after_name = static_cast<const char*>(memchr(ptr, ',', end - ptr));
                if (!comma_after_name) {
                    cerr << "Error: Missing comma after name\n";
                    break;
                }

                string_view name(ptr, comma_after_name - ptr);

                //Store mappings
                if (id >= id_to_name_.size()) {
                    id_to_name_.resize(id + 1);
                }
                id_to_name_[id] = name;
                name_to_id_[name] = id;

                // find the end of the line to skip the currency value
                const char* next_line = static_cast<const char*>(memchr(comma_after_name, '\n', end - comma_after_name));

                if (next_line) {
                    ptr = next_line + 1; // Move to the start of the next line
                } else {
                    ptr = end; // No more lines
                }
            }

        }

        void PreviewFile(const string& filename, size_t max_lines = 5){
            ifstream file(filename);
            string line;
            size_t line_count = 0;
            while (getline(file, line) && line_count < max_lines){
                cout<<line<<"\n";
                line_count++;
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