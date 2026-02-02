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
            //Read file inro buffer
            ifstream file(filename, ios::binary | ios::ate);
            if (!file) {
                cerr << "Error opening file: " << filename << endl;
                return;
            }
            streamsize fileSize = file.tellg();
            dataBuffer.resize(fileSize);
            file.seekg(0, ios::beg);
            file.read(dataBuffer.data(), fileSize);

            //Parse buffer line by line
            char* ptr = dataBuffer.data();
            char* end = ptr + fileSize;

            while(ptr<end){
                char* lineStart = ptr;

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