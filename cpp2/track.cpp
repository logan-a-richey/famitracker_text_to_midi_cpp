// track.cpp

#include <iostream>
#include <string>
#include <sstream>
// #include <vector>
// #include <unordered_map>

#include "track.h"

// ctor
Track::Track(std::string name_, int num_rows_, int speed_, int tempo_) {
    name = name_;
    num_rows = num_rows_;
    speed = speed_;
    tempo = tempo_;
    
    num_cols = 5;
    eff_cols.resize(num_cols);
    for (size_t i = 0; i < num_cols; ++i) {
        eff_cols[i] = 1;
    }
}

std::string Track::to_str() const {
    std::ostringstream oss;
    oss << "{'name': '" << name << "', ";
    oss << "'num_rows': " << num_rows << ", ";
    oss << "'num_cols': " << num_cols << ", ";
    oss << "'speed': " << speed << ", ";
    oss << "'tempo': " << tempo << ", ";
    oss << "'orders': " << "{";
    for (const auto& p : orders) {
        oss << p.first << ": ";
        oss << "[";
        for (size_t i = 0; i < (p.second).size(); ++i) {
            oss << (p.second)[i];
            if (i + 1 < (p.second).size() ) {
                oss << ", ";
            }
        }
        oss << "]";
    }
    oss << "}, ";

    oss << "'num tokens': " << tokens.size();
    oss << "}";
    return oss.str();
}

