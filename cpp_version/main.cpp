// famitracker converter

#include <iostream>
#include <string>

#include "project_reader.h"
#include "project.h"

int main(int argc, char** argv) {
    std::cout << "Welcome to ftm2mid" << std::endl;
    
    Project p;
    ProjectReader pr;
    
    const std::string input_file = argv[1];
    
    pr.read_project(input_file, p);
    
    p.display();

    return 0;
}
