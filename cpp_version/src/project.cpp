// project.cpp

#include "project.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

void Project::display() {
    std::cout << "\n===== Displaying Project =====\n";
    std::cout << "--- Song Information ---\n";
    std::cout << "title: " << title << "\n";
    std::cout << "author: " << author << "\n";
    std::cout << "copyright: " << copyright << "\n";

    std::cout << "\n--- Comments ---\n";
    for (const auto& c : comments) {
        std::cout << c << "\n";
    }
    
    std::cout << "\n--- Global Settings ---\n";
    std::cout << "machine: " << machine << "\n";
    std::cout << "framerate: " << framerate << "\n";
    std::cout << "expansion: " << expansion << "\n";
    std::cout << "vibrato: " << vibrato << "\n";
    std::cout << "split: " << split << "\n";
    std::cout << "n163channels: " << n163channels << "\n";
    
    std::cout << "\n--- Macros ---\n";

    std::cout << "\n--- Instruments ---\n";

    std::cout << "\n--- Tracks ---\n";
}
