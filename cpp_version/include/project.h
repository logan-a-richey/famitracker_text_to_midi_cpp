// project.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "macro.h"
#include "dpcm_sample.h"
#include "groove.h"
#include "instrument.h"
#include "track.h"

struct Project {
    std::string title, author, copyright;
    std::vector<std::string> comments;
    int machine, framerate, expansion, vibrato, split, n163channels;
    
    std::unordered_map<std::string, Macro> macros;
    std::unordered_map<int, DpcmSample> dpcm_samples;
    std::unordered_map<int, Instrument> instruments;
    std::unordered_map<int, Groove> grooves;
    std::unordered_set<int> use_groove;
    std::vector<Track> tracks;

    /*
    Project() {
        macros.reserve(64);
        dpcm_samples.reserve(64);
        instruments.reserve(64);
        grooves.reserve(64);
    }
    */
    
public:
    // Ctor
    Project() = default;

    // Dtor
    ~Project() {
        comments.clear();
        macros.clear();
        dpcm_samples.clear();
        instruments.clear();
        grooves.clear();
        use_groove.clear();
        tracks.clear();
    }

    std::string to_str() const;
};

