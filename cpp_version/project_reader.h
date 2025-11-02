// project_reader.h

#pragma once

#include <string>
#include <unordered_map>
#include "handlers.h"

class Project;

class ProjectReader {
public:
    void init();
    void read_project(const std::string& input_file, Project& project);

private:
    int current_pattern;
    void process_line(Project& project, const std::string& line, const std::string& tag);
    std::unordered_map<std::string, IHandler*> dtable; 
};
