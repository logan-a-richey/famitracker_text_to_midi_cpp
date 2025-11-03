// track.cpp

#include <iostream>
#include <string>
#include <sstream>
// #include <vector>
// #include <unordered_map>

#include "track.h"

#include "container_printing.hpp"

// ctor
Track::Track(const std::string& name_, int num_rows_, int speed_, int tempo_) {
    name = name_;
    num_rows = num_rows_;
    speed = speed_;
    tempo = tempo_;
    
    num_cols = 5;
    eff_cols.resize(num_cols);
    for (int i = 0; i < num_cols; ++i) {
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
    
    int num_orders = orders.size();
    int it = 0;
    for (const auto& pair : orders) {
        oss << pair.first << ": ";
        oss << vector_to_string(pair.second);
        if (it + 1 < num_orders) {
            oss << ", ";
        } 
        it++;
    }
    oss << "}, ";

    oss << "'num tokens': " << tokens.size();
    oss << "}";
    return oss.str();
}

