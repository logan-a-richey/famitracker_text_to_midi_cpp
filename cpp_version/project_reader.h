// project_reader.h

#pragma once

#include <string>

class Project;

class ProjectReader {
public:
    void read_project(const std::string& input_file, Project& p);

private:
    int current_pattern;
// private:
    
};
