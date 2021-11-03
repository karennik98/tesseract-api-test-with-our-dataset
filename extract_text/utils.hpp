#pragma once
#include "OpenXLSX.hpp"

using namespace OpenXLSX;

inline void write_xlsx(const std::string& file_name, const std::vector<std::pair<std::string, std::vector<std::pair<std::string, float>>>> data) {
    XLDocument doc;
    doc.create(file_name);
    auto wks = doc.workbook().worksheet("Sheet1");
    std::vector<std::string> data_types;
    std::vector<std::string> fonts;
    std::vector<std::vector<float>> wers;

    for(const auto& el_i : data) {
        data_types.push_back(el_i.first);
    }
    for(const auto& el_i : data) {
        for(const auto & el_j : el_i.second) {
            fonts.push_back(el_j.first);
        }
        break;
    }
    for(const auto& el_i : data) {
        std::vector<float> tmp;
        for(const auto & el_j : el_i.second) {
            tmp.push_back(el_j.second);
        }
        wers.push_back(tmp);
    }

    int ii = 2;
    for(const auto& el : data_types) {
        wks.cell("A"+std::to_string(ii++)).value() = el;
    }

    int jj = 2;
    for(int i = 0; i <fonts.size(); ++i) {
        wks.cell(1, jj++).value() = fonts[i];
    }

    int k = 2;
    int c = 2;
    for(int i = 0; i < wers.size(); ++i) {
        for(int j = 0; j < wers[i].size(); ++j) {
            wks.cell(k, c+j).value() = wers[i][j];
        }
        k++;
    }

    doc.save();
    doc.close();
}

inline void write_xlsx(const std::string& file_name) {
    std::vector<std::string> headers{"bdhcvhdvchdhcdhcbhd", "bchdvchvhcvdhcbhd", "bcvshvhcvxhvchxvchx", "aoaiosaoxkoxkaoskxcos"};
    std::vector<std::string> subs{"bdhcvhdvchcbhd", "bchvhcvdhcbhd", "bcvshxvchx", "aoaioskxcos", "bdhcvhdvchcbhd", "bchvhcvdhcbhd", "bcvshxvchx", "aoaioskxcos"};
    std::vector<std::vector<float>> wers{
            {1,2,1,2,1,2,1,2},
            {1,2,1,2,1,2,1,2},
            {1,2,1,2,1,2,1,2},
            {1,2,1,2,1,2,1,2}
    };

    XLDocument doc;
    doc.create(file_name);
    auto wks = doc.workbook().worksheet("Sheet1");
    int iii = 2;
    for(const auto& el : headers) {
        wks.cell("A"+std::to_string(iii++)).value() = el;
    }
    int jj = 2;
    for(int ii = 0; ii <subs.size(); ++ii) {
        wks.cell(1, jj++).value() = subs[ii];
    }

    int k = 2;
    int c = 2;
    for(int i = 0; i < wers.size(); ++i) {
        for(int j = 0; j < wers[i].size(); ++j) {
            wks.cell(k, c+j).value() = wers[i][j];
        }
        k++;
    }

    doc.save();
    doc.close();
}

inline void write_file(const std::string& file_name, const std::vector<std::pair<std::string, std::vector<std::pair<std::string, float>>>> data) {
    size_t fonts_count = data.at(0).second.size();
    std::string space(" ", fonts_count);

    std::ofstream file;

    file.open(file_name);
    if(!file.is_open()) {
        throw std::runtime_error("Cant open file");
    }

    for(const auto& el : data) {
        file<<el.first<<" | ";
    }
    file<<std::endl<<std::endl;

    for(const auto& el : data) {
        for(const auto& el_i : el.second) {
            file<<el_i.first<<" ";
        }
        file<<" | ";
    }
    file<<std::endl<<std::endl;

    for(const auto& el : data) {
        for(const auto& el_i : el.second) {
            file<<el_i.second<<" ";
        }
        file<<" | ";
    }
    file<<std::endl;

    file.close();
}