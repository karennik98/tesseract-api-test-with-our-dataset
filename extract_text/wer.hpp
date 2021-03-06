#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <streambuf>


typedef std::vector<std::string> Sentence;

Sentence &split(const std::string &s, char delim, Sentence &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

Sentence split(const std::string &s, char delim) {
    Sentence elems;
    split(s, delim, elems);
    return elems;
}

unsigned int countw(std::string const& str) {
    std::stringstream stream(str);
    return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}

unsigned int edit_distance(const Sentence& s1, const Sentence& s2) {
    const size_t len1 = s1.size();
    const size_t len2 = s2.size();
    std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

    d[0][0] = 0;
    for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
    for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

    for(unsigned int i = 1; i <= len1; ++i)
        for(unsigned int j = 1; j <= len2; ++j)
        {
            d[i][j] = std::min(d[i - 1][j] + 1, d[i][j - 1] + 1);
            d[i][j] = std::min(d[i][j], d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
        }
    return d[len1][len2];
}

float wer_on_text(const std::string& org_file_path, const std::string& ocr_file_path) {
    float wer = 0;
    float total_wc = 0;
    float total_line_no_match = 0;
    float total_line = 0;

    std::ifstream org_file(org_file_path);
    std::ifstream asr_file(ocr_file_path);

    std::string org_line;
    std::string asr_line;

    Sentence s1;
    Sentence s2;

    while (std::getline(org_file, org_line))
    {
        std::getline(asr_file, asr_line);

        if(org_line.compare(asr_line) != 0) {
            total_line_no_match = total_line_no_match + 1;
        }

        total_line = total_line + 1;

        s1 = split(org_line, ' ');
        s2 = split(asr_line, ' ');


        wer = wer + (float)edit_distance(s1, s2);
        total_wc = total_wc + countw(org_line);

    }

    return wer/total_wc;
}

float wer(const std::string& org_line, const std::string& asr_line) {
    float wer = 0;
    float total_wc = 0;

    Sentence s1;
    Sentence s2;

    s1 = split(org_line, ' ');
    s2 = split(asr_line, ' ');

    wer = wer + (float)edit_distance(s1, s2);
    total_wc = total_wc + countw(org_line);

    return wer/total_wc;
}