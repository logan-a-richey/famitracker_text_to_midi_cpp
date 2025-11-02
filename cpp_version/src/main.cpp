// famitracker converter

#include <iostream>
#include <string>

#include "project_reader.h"
#include "project.h"
#include "utils.hpp"

int main(int argc, char** argv) {

    // check for cmdline args
    if (argc < 2) {
        std::cerr << "[Usage] ./main <input_file.txt>" << std::endl;
        exit(1);
    }
    
    const std::string input_file = argv[1];
    
    Project project;
    ProjectReader project_reader;
    
    // parse the input file
    project_reader.read_project(input_file, project);
    
    project.display();

    // TODO sort project data
    // TODO export project data

    std::cout << "Macro labels: " << std::endl;
    for (const auto& pair : project.macros) {
        std::cout << pair.first << ": " << vector_to_string(pair.second.sequence) << std::endl;
    }
    return 0;
}
