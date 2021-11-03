#include "wer.hpp"
#include "utils.hpp"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <regex>

namespace fs = std::experimental::filesystem;

static const std::string img_ext = ".jpg";
static const std::string file_ext = ".txt";

std::vector<std::string> get_subdisr(const std::string &dir) {
    std::vector<std::string> r;
    for (auto &p: fs::directory_iterator(dir))
        if (fs::is_directory(p)) {
            r.push_back(p.path().string());
        }
    return r;
}

std::map<std::string, std::string> get_folder_data(std::string dir) {
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

std::string get_ocr_out(const std::string& image_path ) {
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

std::string get_file_out(const std::string& file_path) {
    std::ifstream file(file_path.c_str());
    if(!file.is_open()) {
        std::string msg = "Cant open file ";
        msg.append(file_path.c_str());
        throw std::runtime_error(msg);
    }

    std::string line;
    std::getline(file, line);
    return line;
}

bool check(std::string str1, std::string str2) {
    std::cout<<"size of str1: "<<str1.size()<<std::endl;
    std::cout<<"size of str2: "<<str2.size()<<std::endl;
    std::cout<<std::endl;
    return str1 == str2;
}

int main(int argc, char *argv[]) {
    setenv("TESSDATA_PREFIX","../../",1);
    std::vector<std::pair<std::string, std::vector<std::string>>> all_dirs;
    std::vector<std::pair<std::string, std::vector<std::pair<std::string, float>>>> sheet_out;

    auto dirs = get_subdisr(argv[1]);
    for (const auto &dir_i: dirs) {
        auto dirs_j = get_subdisr(dir_i);
        std::vector<std::pair<std::string, float>> sheet_it;
        sheet_out.push_back(std::make_pair(dir_i.substr(dir_i.find_last_of('/')+1), sheet_it));
        all_dirs.push_back(std::make_pair(dir_i, dirs_j));
    }

    std::vector<std::pair<std::string, std::pair<std::string, std::map<std::string, std::string>>>> data;
    for (const auto &sub1 : all_dirs) {
        for(const auto& sub2 : sub1.second) {
            auto files = get_folder_data(sub2);
            data.push_back(std::make_pair(sub1.first, std::make_pair(sub2, files)));
        }
    }

    std::string base_out_dir = argv[1];
    base_out_dir.pop_back();
    base_out_dir.append("_out/");
    fs::create_directory(base_out_dir);

    std::vector<std::pair<std::string, float>> sheet_i;
    for(const auto& el_i: data) {
        std::string out_filder = el_i.first;
        out_filder = std::regex_replace(out_filder, std::regex(argv[1]), base_out_dir);
        fs::create_directory(out_filder);

        std::string font_out_dir = el_i.second.first;
        font_out_dir = std::regex_replace(font_out_dir, std::regex(argv[1]), base_out_dir);
        fs::create_directory(font_out_dir);

        std::string ocr_file = font_out_dir;
        std::string orig_file = font_out_dir;
        std::string out_file = font_out_dir;
        out_file.append("/out.txt");
        ocr_file.append("/ocr_out.txt");
        orig_file.append("/orig_out.txt");
        try {
            std::ofstream file_out(out_file);
            std::ofstream file_ocr(ocr_file);
            std::ofstream file_orig(orig_file);
            if(!file_out.is_open()) {
                throw std::runtime_error("Cant open file");
            }
            if(!file_ocr.is_open()) {
                throw std::runtime_error("Cant open file");
            }
            if(!file_orig.is_open()) {
                throw std::runtime_error("Cant open file");
            }
            size_t all_files_count = el_i.second.second.size();
            size_t fail_count = 0;
            for (const auto&[jpg, txt]: el_i.second.second) {
                std::string ocr_out = get_ocr_out(jpg);
                std::string orig_out = get_file_out(txt);
                ocr_out.pop_back();
                file_ocr << ocr_out << std::endl;
                file_orig << orig_out << std::endl;
            }
            file_ocr.close();
            file_orig.close();

            float wer_val = wer_on_text(orig_file, ocr_file);
            file_out << "WER: "<<wer_val<<std::endl<<std::endl;
            file_out << "WER: "<<wer_val * 100 <<"%"<<std::endl;

            for(int i = 0; i < sheet_out.size(); ++i) {
                if(el_i.first.substr(el_i.first.find_last_of('/')+1) == sheet_out[i].first) {
                    std::cout<<el_i.second.first.substr(el_i.second.first.find_last_of('/') + 1)<<" : "<<int(wer_val*100)<<std::endl;
                    sheet_out[i].second.push_back(std::make_pair(el_i.second.first.substr(el_i.second.first.find_last_of('/') + 1),
                                                                 int(wer_val*100)));
                }
            }

            file_out.close();
        } catch (std::exception& ex) {
            std::cout<<"msg: "<<ex.what()<<std::endl;
//            exit(-1);
        }
    }
    for(const auto& el_i : sheet_out) {
        std::cout<<el_i.first<<std::endl;
        for(const auto& el_j : el_i.second) {
            std::cout<<el_j.first<<" : "<<el_j.second<<std::endl;
        }
        std::cout<<std::endl;
    }

    write_xlsx(base_out_dir+ "wer.xlsx", sheet_out);
    return 0;
}