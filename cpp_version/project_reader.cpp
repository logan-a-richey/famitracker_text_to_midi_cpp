// project_reader.cpp

#include "project_reader.h"

#include <string>
#include <fstream>
#include <iostream>

void ProjectReader::read_project(const std::string& input_file, Project& p) {
    std::ifstream fh(input_file);
    if (!fh.is_open()) {
        std::cerr << "[E] could not open file " << input_file << std::endl;
        exit(1);
    }
    std::string line;
    while (std::getline(fh, line)) {
        std::cout << "Line: " << line << "\n";
    }
    fh.close();
}
