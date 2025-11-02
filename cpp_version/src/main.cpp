// famitracker converter

#include <iostream>
#include <string>

#include "project_reader.h"
#include "project.h"

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

    return 0;
}
