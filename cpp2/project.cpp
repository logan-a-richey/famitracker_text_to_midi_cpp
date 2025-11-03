// project.cpp

#include "project.h"

#include <string>
#include <sstream>

std::string Project::to_str() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\n\t'title': " << title << ", ";
    oss << "\n\t'author': " << author << ", ";
    oss << "\n\t'copyright': " << copyright << ", ";

    oss << "\n\t'comments': ";
    oss << "[";
    for (size_t i = 0; i < comments.size(); ++i) {
        oss << "'" << comments[i] << "'";
        if (i + 1 < comments.size() ) {
            oss << ", ";
        }
    }
    oss << "], ";

    oss << "\n\t'machine': " << machine << ", ";
    oss << "\n\t'framerate': " << framerate << ", ";
    oss << "\n\t'expansion': " << expansion << ", ";
    oss << "\n\t'vibrato': " << vibrato << ", ";
    oss << "\n\t'split': " << split << ", ";
    oss << "\n\t'n163channels': " << n163channels << ", ";
    
    oss << "\n\t'tracks': ";
    
    oss << "[\n";
    for (size_t i = 0; i < tracks.size(); ++i) {
        oss << "\t\t" << tracks[i].to_str();
        if (i + 1 < tracks.size() ) {
            oss << ", ";
        }
        oss << "\n";
    }
    oss << "\t]";
    oss << "\n}";
    
    return oss.str();
}

