// project_reader.h

#pragma once 

#include <string>
#include <functional>

struct Project;

class ProjectReader {
public:
    // Ctor
    ProjectReader();

    // Main entry point
    void execute(Project& project, const std::string& input_file);

private:
    // Metadata handlers
    void handle_song_information(Project& project, const std::string& line);
    void handle_global_settings(Project& project, const std::string& line);
       
    // Macro handlers
    void handle_macro(Project& project, const std::string& line);

    // DPCM handlers
    void handle_dpcm_def(Project& project, const std::string& line);
    void handle_dpcm_data(Project& project, const std::string& line);
    
    // Groove handlers
    void handle_groove(Project& project, const std::string& line);
    void handle_use_groove(Project& project, const std::string& line);
    
    // Inst Handlers
    void handle_inst_basic(Project& project, const std::string& line);
    void handle_inst_vrc7(Project& project, const std::string& line);
    void handle_inst_fds(Project& project, const std::string& line);

    // Special handlers
    void handle_key_dpcm(Project& project, const std::string& line);
    void handle_fds_wave(Project& project, const std::string& line);
    void handle_fds_mod(Project& project, const std::string& line);
    void handle_fds_macro(Project& project, const std::string& line);
    void handle_n163_wave(Project& project, const std::string& line);
    
    // Track handlers
    void handle_track(Project& project, const std::string& line);
    void handle_columns(Project& project, const std::string& line);
    void handle_order(Project& project, const std::string& line);
    void handle_pattern(Project& project, const std::string& line);
    void handle_row(Project& project, const std::string& line);
    void process_line(Project& project, const std::string& line);

private:
    // Dispatch table
    using Handler = std::function<void(Project&, const std::string&)>;
    std::unordered_map<std::string, Handler> dtable;
    
    // for persistent variables that need to be remembered as we are scanning the file.
    int current_pattern;
    int current_dpcm_idx;
};

