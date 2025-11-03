// main.cpp

#include "project.h"
#include "project_reader.h"
#include "project_parser.h"
#include "project_exporter.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) 
{
    // get input file
    if (argc < 2) {
        std::cerr << "[Usage Error] ./main.exe <input.txt<" << std::endl;
        exit(1);
    }

    std::string input_file = argv[1];
    std::string output_dir = "output";

    // init data structures
    Project p;
    ProjectReader pr;
    ProjectParser pp;
    ProjectExporter pe;
    // MidiWriter mw;

    // process file
    pr.execute(p, input_file);
    pp.execute(p);
    pe.execute(p, output_dir);
    
    std::cout << "\n[D] Printing Project: \n" << p.to_str() << std::endl;

    return 0;
}
