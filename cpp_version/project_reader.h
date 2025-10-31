// project_reader.h

#pragma once

#include <string>

class Project;

class ProjectReader {
public:
    void read_project(const std::string& input_file, Project& project);

private:
    int current_pattern;

private:
    void process_line(Project& project, const std::string& line, const std::string& tag);
    void handle_song_information(Project& project, const std::string& line, const std::string& tag);
    void handle_global_settings(Project& project, const std::string& line, const std::string& tag);
};
