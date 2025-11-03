//  track.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct Track {
    std::string name;
    int num_rows, num_cols, speed, tempo;
    std::vector<int> eff_cols;
    std::unordered_map<int, std::vector<int>> orders;
    std::unordered_map<std::string, std::string> tokens;
    std::vector<std::string> lines;

public:
    // Ctor
    Track() = default;

    // Ctor2
    Track( const std::string& name_, int num_rows_, int speed_, int tempo_ );
    

    // Dtor
    ~Track() {
        eff_cols.clear();
        orders.clear();
        tokens.clear();
        lines.clear();
    }

    std::string to_str() const;
};

