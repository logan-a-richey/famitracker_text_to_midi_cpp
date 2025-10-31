// project.h

#pragma once

#include <string>
#include <vector>

#include "track.h"

class Project {
public:
    std::string title, author, copyright;
    std::vector<std::string> comments;
    int machine, framerate, expansion, vibrato, split, n163channels;
    std::vector<Track> tracks;
public:
    void display();
};
