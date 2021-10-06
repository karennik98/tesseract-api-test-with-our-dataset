#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
namespace fs = std::experimental::filesystem;

int main()
{
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("", "hye")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }


    std::string path = "/home/karen/master/tess_c++_api_examples/extract_text/images";
    std::vector<std::string> files;
    for (const auto & file : fs::directory_iterator(path)) {
        files.push_back(file.path());
    }
    std::ofstream outfile("out.txt");
    for(const auto& file : files) {
        // Open input image with leptonica library
        Pix *image = pixRead(file.c_str());
        api->SetImage(image);
        // Get OCR result
        outText = api->GetUTF8Text();
        outfile<<file<<": ";
        outfile<<std::string(outText);
        printf("OCR output:\n%s", outText);
        pixDestroy(&image);
    }
    outfile.close();
    // Destroy used object and release memory
    api->End();
    delete api;
    delete[] outText;

    return 0;
}