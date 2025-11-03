// project.h

#pragma once

#include <string>
#include <vector>
// #include <unordered_map>

#include "track.h"

struct Project {
    std::string title, author, copyright;
    std::vector<std::string> comments;
    int machine, framerate, expansion, vibrato, split, n163channels;
    // std::unordered_map<std::string, Macro> macros;
    // std::unordered_map<int, DpcmSample> dpcm_samples;
    // std::unordered_map<int, Instrument> instruments;
    // std::unordered_set<int, Groove> usergroove;
    std::vector<Track> tracks;

public:
    std::string to_str() const;
};

