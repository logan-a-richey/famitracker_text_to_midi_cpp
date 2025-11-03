// project.cpp

#include "project.h"
#include "container_printing.hpp"

#include <string>
#include <sstream>

std::string Project::to_str() const {
    std::ostringstream oss;
    oss << "--- Song Information ---\n";
    oss << "title: " << title << "\n";
    oss << "author: " << author << "\n";
    oss << "copyright: " << copyright << "\n";
    
    if (!comments.empty()) {
        oss << "\n--- Comments ---\n";
        for (const auto& x : comments) {
            oss << x << "\n";
        }
    }

    oss << "\n--- Global Settings ---\n";
    oss << "machine: " << machine << "\n";
    oss << "framerate: " << framerate << "\n";
    oss << "expansion: " << expansion << "\n";
    oss << "vibrato: " << vibrato << "\n";
    oss << "split: " << split << "\n";
    oss << "n163channels: " << n163channels << "\n";
    
    oss << "\n--- Macros ---\n";
    for (const auto& x : macros) {
        oss << "'" << x.first << "'" << ": " << vector_to_string(x.second.sequence) << "\n";
    }

    oss << "\n--- Instruments ---\n";
    for (const auto& x : instruments) {
        oss << x.first << ": '" << x.second.name << "'\n";
    }
     
    oss << "\n--- Tracks ---\n";
    for (size_t i = 0; i < tracks.size(); ++i) {
        oss << "Track " << i << ": " << "'" << tracks[i].name << "'" << "\n";
    }

    return oss.str();
}

