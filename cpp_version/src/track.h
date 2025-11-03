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

public:
    // ctor
    Track(std::string name_, int num_rows_, int speed_, int tempo_);
    std::string to_str() const;
};

