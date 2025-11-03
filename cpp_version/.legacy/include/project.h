// project.h

#pragma once

#include <string>
#include <vector>
#include <map>

#include "track.h"
#include "macro.h"
#include "instrument.h"

class Project {
public:
    std::string title, author, copyright;
    std::vector<std::string> comments;
    int machine, framerate, expansion, vibrato, split, n163channels;
    std::map<std::string, Macro> macros;
    std::map<std::string, BaseInst> instruments;
    std::vector<Track> tracks;

public:
    void display();
};
