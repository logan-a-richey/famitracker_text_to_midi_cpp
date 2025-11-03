// project_reader.h

#pragma once 

#include <string>
#include <functional>

struct Project;

class ProjectReader {
public:
    ProjectReader();
    void execute(Project& project, const std::string& input_file);

private:
    void handle_song_information(Project& project, const std::string& line, const std::string& tag);
    void handle_global_settings(Project& project, const std::string& line, const std::string& tag);
    
    void handle_macro(Project& project, const std::string& line, const std::string& tag);
    
    void handle_track(Project& project, const std::string& line, const std::string& tag);
    void handle_columns(Project& project, const std::string& line, const std::string& tag);
    void handle_order(Project& project, const std::string& line, const std::string& tag);
    void handle_pattern(Project& project, const std::string& line, const std::string& tag);
    void handle_row(Project& project, const std::string& line, const std::string& tag);
    void process_line(Project& project, const std::string& line, const std::string& tag);

private:
    // using Handler = void(ProjectReader::*)(Project&, const std::string&, const std::string&);
    // std::unordered_map<std::string, Handler> dtable;
    
    std::unordered_map<
        std::string, 
        std::function<void(Project&, const std::string&, const std::string&)>
    > dtable;

    int current_pattern;

};
