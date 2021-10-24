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
    std::vector<std::pair<std::string, std::vector<std::string>>> all_dirs;
    auto dirs = get_subdisr(argv[1]);
    for (const auto &dir_i: dirs) {
        auto dirs_j = get_subdisr(dir_i);
        all_dirs.push_back(std::make_pair(dir_i, dirs_j));
    }

    std::vector<std::pair<std::string, std::pair<std::string, std::map<std::string, std::string>>>> data;
    for (const auto &el: all_dirs) {
        for(const auto&  el_j : el.second) {
            auto files = get_folder_data(el_j);
            data.push_back(std::make_pair(el.first, std::make_pair(el_j, files)));
        }
    }

    std::string base_out_dir = argv[1];
    base_out_dir.pop_back();
    base_out_dir.append("_out/");
    fs::create_directory(base_out_dir);

    for(const auto& el_i: data) {
        std::string out_filder = el_i.first;
        out_filder = std::regex_replace(out_filder, std::regex(argv[1]), base_out_dir);
        fs::create_directory(out_filder);

        std::string font_out_dir = el_i.second.first;
        font_out_dir = std::regex_replace(font_out_dir, std::regex(argv[1]), base_out_dir);
        fs::create_directory(font_out_dir);

        std::string out_file = font_out_dir;
        out_file.append("/out.txt");
        try {
            std::ofstream file(out_file);
            if(!file.is_open()) {
                throw std::runtime_error("Cant open file");
            }
            size_t all_files_count = el_i.second.second.size();
            size_t fail_count = 0;
            for (const auto&[jpg, txt]: el_i.second.second) {
                std::string ocr_out = get_ocr_out(jpg);
                ocr_out.pop_back();
                std::string file_out = get_file_out(txt);
                if (!check(file_out, ocr_out)) {
                    fail_count ++;
                    file << "jpg file:  "<<jpg<<std::endl;
                    file << "text file: "<<txt<<std::endl;
                    file << "ocr out:   "<<ocr_out<<std::endl;
                    file << "text out:  "<<file_out<<std::endl<<std::endl;
                    std::cout << "ocr_out: " << ocr_out << std::endl;
                    std::cout << "file_out: " << file_out << std::endl;
                    std::cout << std::endl;
                }
            }
            file << "number of all files: "<<all_files_count<<std::endl;
            file << "number of fails:     "<<fail_count<<std::endl;
            file << "fails percent:       "<<float((fail_count*100)/all_files_count)<<"%"<<std::endl;
            file.close();
        } catch (std::exception& ex) {
            std::cout<<"msg: "<<ex.what()<<std::endl;
//            exit(-1);
        }
    }

    return 0;
}