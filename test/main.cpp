#include "wer.hpp"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <experimental/filesystem>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>

namespace fs = std::experimental::filesystem;

static const std::string img_ext = ".tif";
static const std::string file_ext = ".gt.txt";

std::map<std::string, std::string> get_folder_data(std::string dir) {
    std::vector<std::string> all_files;
    std::map<std::string, std::string> data;
    for (const auto &entry: fs::directory_iterator(dir)) {
        all_files.emplace_back(entry.path());
    }

    for(auto el = all_files.begin(); el != all_files.end(); el ++) {
        std::string image, file;
        auto slash_pos = el->find_last_of('/');
        auto file_full_name = el->substr(slash_pos + 1, el->size());

        if(file_full_name.find(img_ext) != std::string::npos) {
            image = *el;
            all_files.erase(el);
        } else if(file_full_name.find(file_ext) != std::string::npos) {
            file = *el;
            all_files.erase(el);
        } else {
            std::cout<<"[DEBUG]: INVALID FILE\n";
        }

        auto name = file_full_name.substr(0, file_full_name.find_first_of('.'));

        for( auto el_j = all_files.begin(); el_j != all_files.end(); ++el_j) {
            auto slash_pos_j = el_j->find_last_of('/');
            auto file_full_name_j = el_j->substr(slash_pos_j + 1, el_j->size());
            auto name_j = file_full_name_j.substr(0, file_full_name_j.find_first_of('.'));
            if(name == name_j) {
                if (image.empty()) {
                    std::string full = name + img_ext;
                    if (el_j->find(full) != std::string::npos) {
                        image = *el_j;
                        all_files.erase(el_j);
                    }
                } else if (file.empty()) {
                    std::string full = name + file_ext;
                    if (el_j->find(full) != std::string::npos) {
                        file = *el_j;
                        all_files.erase(el_j);
                    }
                } else {
                    std::cout << "[ERROR]: INVALID FILE\n";
                }
                if (!image.empty() && !file.empty()) {
                    break;
                }
            }
        }

        if(!image.empty() && !file.empty()) {
            data.insert({image, file});
        } else {
            std::cout<<"[ERROR]: INVALID FILES\n";
        }
    }

    return data;
}

std::string get_ocr_out(const std::string& image_path) {
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("", "hye")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    // Open input image with leptonica library
    Pix *image = pixRead(image_path.c_str());
    api->SetImage(image);

    // Get OCR result
    outText = api->GetUTF8Text();
    pixDestroy(&image);

    // Destroy used object and release memory
    std::string out = {outText};
    api->End();
    delete api;
    delete[] outText;

    return out;
}

std::string get_file_out(const std::string& path) {
    std::string line;

    std::ifstream my_file (path);
    if (my_file.is_open()) {
        getline (my_file,line);
    } else {
        std::cout << "Unable to open file";
    }

    return line;
}

int main(int argc, char** argv) {
    setenv("TESSDATA_PREFIX","../../",1);
    if(argc < 1) {
        std::cerr<<"[ERROR]: invalid arguments\n";
        std::cout<<"[INFO]: data folder";
        exit(1);
    }

    std::ofstream file("incorect_files.txt");
    if(!file.is_open()) {
        std::cerr<<"[ERROR]: can poen file\n";
        exit(1);
    }

    auto data = get_folder_data(argv[1]);
    for(const auto& [key, value] : data) {
        auto ocr_out = get_ocr_out(key);
        auto file_out = get_file_out(value);

        auto wer_out = wer(file_out, ocr_out);

        if(wer_out == 1) {
            file<<"[IMAGE]: "<<key<<std::endl;
            file<<"[FILE]: "<<value<<std::endl;
            file<<"\n";
        }

        std::cout << "[INFO]: Original string: " << file_out << std::endl;
        std::cout << "[INFO]: OCR string: " << ocr_out << std::endl;
        std::cout << "[INFO]: WER: " << wer_out << std::endl;
    }

    file.close();
    return 0;
}
