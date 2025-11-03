// project_reader.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>

#include "project.h"
#include "project_reader.h"
#include "string_helpers.hpp"
#include "macro.h"
#include "instrument.h"
#include "track.h"

void handle_song_information(Project& p, const std::string& line, const std::string& tag) {
    std::string value = get_quote(line);
    if (value.empty()) { return; }

    if (tag == "TITLE") { p.title = value;    }
    else if (tag == "AUTHOR") { p.author = value; }
    else if (tag == "COPYRIGHT") { p.copyright = value; }
    else if (tag == "COMMENT") { p.comments.push_back(value); }
    else { std::cerr << "Unknown tag: " << tag << std::endl; }
}

void handle_global_settings(Project& p, const std::string& line, const std::string& tag) {
    // TODO use regex to get the number value
    std::stringstream ss(line);
    std::string first_word;
    int value;
    
    ss >> first_word;
    ss >> value;

    if (tag == "MACHINE") { p.machine = value; }
    else if (tag == "FRAMERATE") { p.framerate = value; }
    else if (tag == "EXPANSION") { p.expansion = value; }
    else if (tag == "VIBRATO") { p.vibrato = value; }
    else if (tag == "SPLIT") { p.split = value; }
    else if (tag == "N163CHANNELS") { p.n163channels = value; }
    else { std::cerr << "Unknown tag: " << tag << std::endl; }
}

void handle_track(Project& p, const std::string& line, const std::string& tag) {
    std::string first_word;
    int num_rows, speed, tempo;

    std::stringstream ss(line);
    ss >> first_word;
    ss >> num_rows >> speed >> tempo;
    std::string name = get_quote(line);
    
    Track t(name, num_rows, speed, tempo);

    p.tracks.push_back(t);
}

void handle_columns(Project& p, const std::string& line, const std::string& tag) {
    if (p.tracks.empty()) {
        std::cerr << "[E] Cannot process COLUMNS until a Track has been initialized." << std::endl;
        return;
    }
    Track& t = p.tracks.back();
}

void handle_order(Project& p, const std::string& line, const std::string& tag) {
    if (p.tracks.empty()) {
        std::cerr << "[E] Cannot process ORDER until a Track has been initialized." << std::endl;
        return;
    }
    Track& t = p.tracks.back();
}

void handle_pattern(Project& p, const std::string& line, const std::string& tag) {
    if (p.tracks.empty()) {
        std::cerr << "[E] Cannot process PATTERN until a Track has been initialized." << std::endl;
        return;
    }
    Track& t = p.tracks.back();
}

void handle_row(Project& p, const std::string& line, const std::string& tag) {
    if (p.tracks.empty()) {
        std::cerr << "[E] Cannot process ROW until a Track has been initialized." << std::endl;
        return;
    }
    Track& t = p.tracks.back();
}

void process_line(Project& p, const std::string& line, const std::string& tag) {
    // process a Famitracker Line
    static std::unordered_set<std::string> song_information_tags = {
        "TITLE", "AUTHOR", "COPYRIGHT", "COMMENT"
    };
    static std::unordered_set<std::string> global_settings_tags = {
        "MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS"
    };
    
    if (song_information_tags.find(tag) != song_information_tags.end()) {
        handle_song_information(p, line, tag);
    }
    else if (global_settings_tags.find(tag) != global_settings_tags.end()) {
        handle_global_settings(p, line, tag);
    }
    else if (tag == "TRACK") {
        handle_track(p, line, tag);
    } 
    else if (tag == "COLUMNS") {
        handle_columns(p, line, tag);
    }
    else if (tag == "ORDER") {
        handle_order(p, line, tag);
    }
    else if (tag == "PATTERN") {
        handle_pattern(p, line, tag);
    }
    else if (tag == "ROW") {
        handle_row(p, line, tag);
    }
    else {
        // std::cout << "Unknown tag: " << tag << std::endl;
    }

    // std::cout << line << std::endl;
}

// *****************************************************************************

void ProjectReader::execute(Project& p, const std::string& input_file) {
    std::ifstream fh(input_file);
    if (!fh.is_open()) {
        std::cerr << "[E] Could not open file: " << input_file << std::endl;
        exit(1);
    }

    std::string line;

    while (std::getline(fh, line)) {
        std::string first_word;
        std::stringstream ss(line);
        ss >> first_word;

        // skip blank lines and comment lines
        if (first_word.empty()) {
            continue;
        }
        if (first_word[0] == '#') {
            continue;
        }
        process_line(p, line, first_word);
    }
}

