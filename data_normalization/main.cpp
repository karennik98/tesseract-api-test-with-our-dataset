#include <iostream>

#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <regex>


namespace fs = std::experimental::filesystem;


static const std::string img_ext = ".tif";
static const std::string file_ext = ".gt.txt";

using data_t = std::vector<std::pair<std::string, std::pair<std::string, std::map<std::string, std::string>>>>;


std::vector<std::string> get_subdisr(const std::string &dir) {
    std::vector<std::string> r;
    for (auto &p: fs::directory_iterator(dir))
        if (fs::is_directory(p)) {
            r.push_back(p.path().string());
        }
    return r;
}


std::map<std::string, std::string> get_folder_data(const std::string& dir) {
    std::vector<std::string> temp_data;
    for (const auto &entry: fs::directory_iterator(dir)) {
        temp_data.emplace_back(entry.path());
    }

    std::map<std::string, std::string> data;
    for(auto& el : temp_data) {
        if( size_t dot_pos = el.find(img_ext); dot_pos != std::string::npos) {
            if(data.find(el) == data.end()) {
                size_t slash_pos = el.find_last_of("/");
                size_t end_pos = dot_pos - slash_pos - 1;
                std::string name = el.substr(++slash_pos, end_pos);
                for(auto& el_j : temp_data) {
                    if(el_j.find(name + file_ext) != std::string::npos) {
                        data[el] = el_j;
                    }
                }
            }
        }
    }

    return data;
}


void print_data(data_t data) {
    for(const auto& el : data) {
        std::cout<<el.first<<std::endl;
        std::cout<<el.second.first<<std::endl;
        for(const auto& [key, value] : el.second.second) {
            std::cout<<"key: "<<key<<std::endl;
            std::cout<<"value: "<<value<<std::endl;
        }
    }
}

void rename_and_copy(data_t data, const std::string& destination) {
    int step = 0;
    for(auto& el : data) {
        for(auto& [key, value] : el.second.second) {
            std::string step_str = std::to_string(step++);

            std::string key_extention = key.substr(key.find_last_of("."));
            std::string value_extention = value.substr(value.find_last_of("."));

            std::string new_key = destination + step_str + img_ext;
            std::string new_value = destination + step_str + file_ext;

            rename(key.c_str(), new_key.c_str());
            rename(value.c_str(), new_value.c_str());
        }
    }
}


int main(int argc, char** argv) {
    std::vector<std::pair<std::string, std::vector<std::string>>> all_dirs;
    data_t data;

    auto dirs = get_subdisr(argv[1]);
    for (const auto &dir_i: dirs) {
        auto dirs_j = get_subdisr(dir_i);
        all_dirs.push_back(std::make_pair(dir_i, dirs_j));
    }

    for (const auto &sub1 : all_dirs) {
        for(const auto& sub2 : sub1.second) {
            auto files = get_folder_data(sub2);
            data.push_back(std::make_pair(sub1.first, std::make_pair(sub2, files)));
        }
    }

    // print_data(data);

    rename_and_copy(data, argv[2]);
    return 0;
}