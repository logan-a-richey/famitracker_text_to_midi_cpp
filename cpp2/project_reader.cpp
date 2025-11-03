// project_reader.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>

#include "project.h"
#include "project_reader.h"
#include "string_helpers.hpp"
#include "macro.h"
#include "constants.h"
#include "instrument.h"
#include "track.h"

ProjectReader::ProjectReader() {
    static std::unordered_set<std::string> song_information_tags = { "TITLE", "AUTHOR", "COPYRIGHT", "COMMENT" };
    static std::unordered_set<std::string> global_settings_tags = { "MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS" };
    static std::unordered_set<std::string> macro_tags = { "MACRO", "MACROVRC6", "MACRON163", "MACROS5B"};
    
    // init dispatch table
    dtable.clear();
    
    // register functions
    for (const auto& tag : song_information_tags) {
        dtable[tag] = [this](Project& project, const std::string& line, const std::string& tag) { handle_song_information(project, line, tag); };
    }
    for (const auto tag : global_settings_tags) {
        dtable[tag] = [this](Project& project, const std::string& line, const std::string& tag) { handle_global_settings(project, line, tag); };
    }
    for (const auto tag : macro_tags) {
        dtable[tag] = [this](Project& project, const std::string& line, const std::string& tag) { handle_macro(project, line, tag); };
    }

    dtable["TRACK"] = [this](Project& project, const std::string& line, const std::string& tag) { handle_track(project, line, tag); };
    dtable["COLUMNS"] = [this](Project& project, const std::string& line, const std::string& tag) { handle_columns(project, line, tag); };
    dtable["ORDER"] = [this](Project& project, const std::string& line, const std::string& tag) { handle_order(project, line, tag); };
    dtable["PATTERN"] = [this](Project& project, const std::string& line, const std::string& tag) { handle_pattern(project, line, tag); };
    dtable["ROW"] = [this](Project& project, const std::string& line, const std::string& tag) { handle_row(project, line, tag); };
}

void ProjectReader::handle_song_information(Project& project, const std::string& line, const std::string& tag) {
    std::string value = get_quote(line);
    if (value.empty()) { return; }

    if (tag == "TITLE") { project.title = value;    }
    else if (tag == "AUTHOR") { project.author = value; }
    else if (tag == "COPYRIGHT") { project.copyright = value; }
    else if (tag == "COMMENT") { project.comments.push_back(value); }
    else { std::cerr << "Unknown tag: " << tag << std::endl; }
}

void ProjectReader::handle_global_settings(Project& project, const std::string& line, const std::string& tag) {
    // TODO use regex to get the number value
    std::stringstream ss(line);
    std::string word;
    int value;
    
    ss >> word;
    ss >> value;

    if (tag == "MACHINE") { project.machine = value; }
    else if (tag == "FRAMERATE") { project.framerate = value; }
    else if (tag == "EXPANSION") { project.expansion = value; }
    else if (tag == "VIBRATO") { project.vibrato = value; }
    else if (tag == "SPLIT") { project.split = value; }
    else if (tag == "N163CHANNELS") { project.n163channels = value; }
    else { std::cerr << "Unknown tag: " << tag << std::endl; }
}

void ProjectReader::handle_macro(Project& project, const std::string& line, const std::string& tag) {
    static std::unordered_map<std::string, InstrumentFamily> inst_t_map = {
        {"MACRO", INST_2A03},
        {"MACROVRC6", INST_VRC6},
        {"MACRON163", INST_N163},
        {"MACROS5B", INST_S5B}
    };
    static std::unordered_map<int, MacroType> macro_t_map = {
        {0, VOL},
        {1, ARP},
        {2, PIT},
        {3, HPI},
        {4, DUT}
    };

    std::stringstream ss(line);
    std::string word;
    ss >> word;

    int type, index, loop, release, setting;
    ss >> type >> index >> loop >> release >> setting;
    
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> sequence = get_int_list(text_after_colon);

    InstrumentFamily inst_t = inst_t_map[tag];
    MacroType macro_t = macro_t_map[type];
    
    std::string macro_key = generate_macro_key(inst_t, macro_t, index);
    
    // TODO
    // Macro m(inst_t, macro_t, index, loop, release, setting, std::move(sequence));
    
    Macro m;
    m.inst_t = inst_t;
    m.macro_t = macro_t;
    m.index = index;
    m.loop = loop;
    m.release = release;
    m.setting = setting; 
    m.sequence = std::move(sequence);

    project.macros[macro_key] = m;
}

void ProjectReader::handle_track(Project& project, const std::string& line, const std::string& tag) {
    std::string word;
    int num_rows, speed, tempo;

    std::stringstream ss(line);
    ss >> word;
    ss >> num_rows >> speed >> tempo;
    std::string name = get_quote(line);
    
    Track track(name, num_rows, speed, tempo);
    
    // std::cout << "Added track: " << track.name << std::endl;
    project.tracks.push_back(std::move(track));
}

void ProjectReader::handle_columns(Project& project, const std::string& line, const std::string& tag) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process COLUMNS until a Track has been initialized." << std::endl;
        exit(1);
    }
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_int_list(text_after_colon);
    int num_cols = numbers.size();

    Track& t = project.tracks.back();
    t.num_cols = num_cols;
    t.eff_cols = std::move(numbers);
}

void ProjectReader::handle_order(Project& project, const std::string& line, const std::string& tag) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process ORDER until a Track has been initialized." << std::endl;
        exit(1);
    }
    std::stringstream ss(line);
    std::string word;
    ss >> word;
    ss >> word;
    int order_idx = convert_hex_str_to_int(word);

    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_hex_list(text_after_colon);

    Track& t = project.tracks.back();
    t.orders[order_idx] = std::move(numbers);
}

void ProjectReader::handle_pattern(Project& project, const std::string& line, const std::string& tag) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process PATTERN until a Track has been initialized." << std::endl;
        exit(1);
    }
    Track& t = project.tracks.back();

    std::stringstream ss(line);
    std::string word;
    ss >> word;
    ss >> word;
    current_pattern = convert_hex_str_to_int(word);
}

void ProjectReader::handle_row(Project& project, const std::string& line, const std::string& tag) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process ROW until a Track has been initialized." << std::endl;
        exit(1);
    }
    Track& current_track = project.tracks.back();

    std::stringstream ss(line);
    std::string word;
    ss >> word;
    ss >> word;

    int row_idx = convert_hex_str_to_int(word);

    std::vector<std::string> tokens = get_colon_fields(line);
    // std::cout << "[D] tokens.size = " << tokens.size() << std::endl;

    // Add tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (contains_only_spaces_and_periods(tokens[i])) {
            // std::cout << "Skipping token: " << tokens[i] << std::endl;
            continue;
        }
        std::string token_key = generate_token_key(current_pattern, row_idx, i);
        current_track.tokens[token_key] = tokens[i];
        // std::cout << "[D] Added item: key='" << token_key << "', val='" << tokens[i] << "'" << std::endl;
    }
}

void ProjectReader::process_line(Project& project, const std::string& line, const std::string& tag) {
    // Process a Famitracker Line.
   
    // Find correct Handler in dispatch table and call the method.
    auto it = dtable.find(tag);
    if (it != dtable.end()) {
        // std::cout << "Found! Handling tag: " << tag << std::endl;
        it->second(project, line, tag);
    }  
}

// *****************************************************************************

void ProjectReader::execute(Project& project, const std::string& input_file) {
    // Main method to call. Entry point to read input_file data into Project.

    std::ifstream fh(input_file);
    if (!fh.is_open()) {
        std::cerr << "[E] Could not open file: " << input_file << std::endl;
        exit(1);
    }

    std::string line;

    while (std::getline(fh, line)) {
        std::string word;
        std::stringstream ss(line);
        ss >> word;

        // Skip blank lines and comment lines.
        if (word.empty()) {
            continue;
        }
        if (word[0] == '#') {
            continue;
        }
        process_line(project, line, word);
    }
}

