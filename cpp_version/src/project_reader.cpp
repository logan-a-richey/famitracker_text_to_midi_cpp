// project_reader.cpp

#include "project_reader.h"

#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

#include "string_helpers.h"

#include "project.h"

// -----------------------------------------------------------------------------

void ProjectReader::init() {
    current_pattern = 0;

    static const std::list<std::string> song_information_tags = { 
        "TITLE", "AUTHOR", "COPYRIGHT", "COMMENT"
    };
    static const std::list<std::string> global_settings_tags = {
        "MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS"
    };
    static const std::list<std::string> macro_tags = {
        "MACRO", "MACROVRC6", "MACRON163", "MACROS5B"
    }; 
    static const std::list<std::string> instrument_tags = {
        "INST2A03", "INSTVRC6", "INSTVRC7", "INSTFDS", "INSTN163", "INSTS5B"
    };
    static const std::list<std::string> track_tags = {
        "TRACK", "COLUMNS", "ORDER", "PATTERN", "ROW"
    };

    static SongInformationHandler song_information_handler;
    static GlobalSettingsHandler global_settings_handler;
    static MacroHandler macro_handler;
    static InstrumentHandler instrument_handler;
    static TrackHandler track_handler;

    dtable.clear();
    
    // link tags to functions
    for (const auto& tag : song_information_tags) { 
        dtable[tag] = &song_information_handler; 
    }
    for (const auto& tag : global_settings_tags) { 
        dtable[tag] = &global_settings_handler; 
    }
    for (const auto& tag : macro_tags) { 
        dtable[tag] = &macro_handler; 
    }
    for (const auto& tag : instrument_tags) { 
        dtable[tag] = &instrument_handler; 
    }
    /*
    for (const auto& tag : track_tags) { 
        dtable[tag] = &track_handler; 
    }
    */
}

void ProjectReader::process_line(Project& project, const std::string& line, const std::string& tag) {
    // std::cout << "[D] Processing line: " << line << std::endl;

    auto it = dtable.find(tag);
    if (it != dtable.end()) {
        // std::cout << "[D] Found key : " << tag << std::endl;
        (it->second)->handle(project, line, tag);
    }
}

void ProjectReader::read_project(const std::string& input_file, Project& project) {
    // project init
    init();
    
    std::ifstream fh(input_file);
    if (!fh.is_open()) {
        std::cerr << "[E] could not open file " << input_file << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(fh, line)) {
        // std::cout << "Line: " << line << "\n";
        
        std::stringstream ss(line);
        std::string tag;
        ss >> tag;
        
        // skip blank lines
        if (tag.empty()) { continue; }
        
        // skip comment lines 
        if (tag[0] == '#') { continue; }

        // std::cout << "First word = " << tag << std::endl;
        process_line(project, line, tag);
    }
    fh.close();
}
