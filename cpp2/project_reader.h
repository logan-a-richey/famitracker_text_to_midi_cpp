// project_reader.h

#pragma once 

#include <string>

struct Project;

class ProjectReader {
public:
    void execute(Project& p, const std::string& input_file);
private:
    int current_pattern;
};
