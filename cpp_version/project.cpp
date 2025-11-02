// project.cpp

#include "project.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

void Project::display() {
    std::cout << "--- Song Information ---\n";
    std::cout << "title: " << title << "\n";
    std::cout << "author: " << author << "\n";
    std::cout << "copyright: " << copyright << "\n";

    std::cout << "--- Comments ---\n";
    for (const auto& c : comments) {
        std::cout << c << "\n";
    }
    
    std::cout << "--- Global Settings ---\n";
    std::cout << "machine: " << machine << "\n";
    std::cout << "framerate: " << framerate << "\n";
    std::cout << "expansion: " << expansion << "\n";
    std::cout << "vibrato: " << vibrato << "\n";
    std::cout << "split: " << split << "\n";
    std::cout << "n163channels: " << n163channels << "\n";
    
    std::cout << "--- Macros ---\n";

    std::cout << "--- Instruments ---\n";

    std::cout << "--- Tracks ---\n";
}
