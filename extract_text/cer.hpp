#pragma once

#include "levenshtein.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

std::string readFile2(const std::string &fileName)
{
    std::ifstream ifs(fileName.c_str(), std::ios::in
    | std::ios::binary
    | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}

double cer_on_text(const std::string& org_file_path, const std::string& ocr_file_path) {
    std::string org_line = readFile2(org_file_path);
    std::string ocr_line = readFile2(ocr_file_path);

    org_line.erase(remove(org_line.begin(), org_line.end(), '\n'), org_line.end());
    ocr_line.erase(remove(ocr_line.begin(), ocr_line.end(), '\n'), ocr_line.end());

    org_line.erase(remove(org_line.begin(), org_line.end(), ' '), org_line.end());
    ocr_line.erase(remove(ocr_line.begin(), ocr_line.end(), ' '), ocr_line.end());

    return levenshtein_distance(org_line.c_str(), org_line.size(), ocr_line.c_str(), ocr_line.size()) / org_line.size() * 100;
}