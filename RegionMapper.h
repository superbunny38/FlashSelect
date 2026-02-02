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
            //
        }

        //
        string_view GetName(int id) const{

        }

        int GetId(string_view name) const{

        }
    
    private:
        vector<char> dataBuffer;
        vector<string_view> id_to_name_;
        unordered_map<string_view, int> name_to_id_;
};