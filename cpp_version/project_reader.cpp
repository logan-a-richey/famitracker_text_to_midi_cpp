// project_reader.cpp

#include "project_reader.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

std::string get_quote(const std::string& input_string) {
    int start = 0;
    int stop = 0;

    for (int i = 0; i <= input_string.length(); ++i) {
        if (input_string[i] == '\"') {
            if (!start) { 
                start = i; 
            }
            stop = i;
        }
    }
    
    // don't include the quotes
    start++;
    // stop--;

    // error check
    if (stop < start) {
        return "";
    }
    int length = stop - start;
    if (length <= 0) {
        return "";
    }

    // return substring
    return input_string.substr(start, stop - start);
}

void ProjectReader::handle_song_information(Project& project, const std::string& line, const std::string& tag) {
    std::cout << "handle_song_information: " << tag << std::endl;
    std::cout << "quote = " << get_quote(line) << std::endl;
}

void ProjectReader::handle_global_settings(Project& project, const std::string& line, const std::string& tag) {
    std::cout << "handle_global_settings: " << tag << std::endl;
}


void ProjectReader::process_line(Project& project, const std::string& line, const std::string& tag) {
    static const std::unordered_set<std::string> song_information_tags = { "TITLE", "AUTHOR", "COPYRIGHT"};
    static const std::unordered_set<std::string> global_settings_tags = {"MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS"};

    using fptr = void(ProjectReader::*)(Project& project, const std::string& line, const std::string& tag);
    static const std::unordered_map<std::string, fptr> dtable = {
        {"TITLE", this->handle_song_information},
        {"AUTHOR", this->handle_song_information},
        {"COPYRIGHT", this->handle_song_information}
    };
  
    // if item in map, call function
    
    // handle project metadata
    if (song_information_tags.find(tag) != song_information_tags.end()) {
        handle_song_information(project, line, tag);
    } 
    else if (tag == "COMMENT") {
        std::cout << "Handling comment" << std::endl;
    }
    else if (global_settings_tags.find(tag) != global_settings_tags.end()) {
        handle_global_settings(project, line, tag);
    } 

    // handle track data
    else if (tag == "TRACK") {
        std::cout << "Handling: " << tag << std::endl;
    }
    else if (tag == "COLUMNS") {
        std::cout << "Handling: " << tag << std::endl;
    }
    else if (tag == "ORDER") {
        std::cout << "Handling: " << tag << std::endl;
    }
    else if (tag == "PATTERN") {
        std::cout << "Handling: " << tag << std::endl;
    }
    else if (tag == "ROW") {
        std::cout << "Handling: " << tag << std::endl;
    }
    else {
        std::cout << "[W] Ignoring unknown line: " << line << std::endl;
    }
}

void ProjectReader::read_project(const std::string& input_file, Project& project) {
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
        if (tag.empty()) {
            continue;
        }
        
        // skip comment lines 
        if (tag[0] == '#') {
            continue;
        }

        // std::cout << "First word = " << tag << std::endl;
        process_line(project, line, tag);

    }
    fh.close();
}
