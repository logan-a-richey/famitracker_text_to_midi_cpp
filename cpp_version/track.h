// track.h

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <array>

class Track {
public:
    std::string name;
    int num_rows, num_cols, speed, tempo;
    std::vector<int> eff_cols;
    std::unordered_map<std::string, std::vector<int>> orders;
    std::unordered_map<std::string, std::string> tokens;
};
