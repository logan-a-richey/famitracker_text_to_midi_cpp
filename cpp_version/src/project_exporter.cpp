// project_exporter.cpp

#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <algorithm> // For std::transform
#include <cctype>    // For std::isalnum, std::tolower
#include <regex>     // For std::regex_replace

#include "project_exporter.h"
#include "project.h"
#include "string_helpers.h"
#include "midi_writer.h"

template<typename T>
void print_vector(const std::vector<T>& vec) 
{
    std::ostringstream oss;
    oss << "[";
    typename std::vector<T>::const_iterator it = vec.begin();
    if (it != vec.end()) {
        oss << "'" << *it << "'";
        ++it;
    }
    while (it != vec.end()) {
        oss << ", " << *it << "'";
        ++it;
    }
    oss << "]";
    std::cout << oss.str() << std::endl;
}

int convert_note_string_to_int(const std::string& note_part) {
    static const std::unordered_map<char, int> letter_mapping = {
        {'C', 0},
        {'D', 2},
        {'E', 4},
        {'F', 5},
        {'G', 7},
        {'A', 9},
        {'B', 11}
    };
    int midi_pitch = 0;

    // Handle note letter
    auto it = letter_mapping.find(note_part[0]);
    if (it == letter_mapping.end() ) {
        return -1;
    }
    midi_pitch += *it;

    // Handle note accidental
    if (note_part[1] == '#') {
        midi_pitch++;
    } else if  (note_part[1] == 'b') {
        midi_pitch--;
    }

    // Handle note octave
    int octave = std::atoi(note_part[2]);
    midi_pitch += ( (octave + 1) * 12);
    
    return midi_pitch;
}

void ProjectExporter::export_track(const Track& track, const std::string& output_file_path) 
{
    MidiWriter mw;
    mw.add_bpm(0, 0, 150); // track, time, bpm
    mw.add_time_signature(0, 0, 4, 4); // track, time, num, den
    
    std::cout << "[d] Inside of export_track: " << track.name << std::endl;
    
    // TODO
    std::vector<ColContext> contexts;
    contexts.reserve(track.num_cols);

    int32_t line_count = 0;

    for (const auto& line : track.lines) {
        std::vector<std::string> tokens = get_colon_fields(line);

        for (std::size_t j = 0; j < tokens.size(); ++j)
        {
            // NOTE 480 ticks per quarter note. 120 = 16th note.
            std::string token = tokens[j];
            std::stringstream iss(token);

            std::string note_part, inst_part, vol_part;
            iss >> note_part >> inst_part >> vol_part;
            
            int res = convert_note_string_to_int(note_part);
            if (res < 0) {
                continue;
            }

            uint32_t track_idx = j;
            uint32_t channel = j % 2;
            uint32_t start = line_count * 120;
            uint32_t duration = 120;
            uint32_t pitch = res;
            uint32_t velocity = 120;

            mw.add_note(track_idx, channel, start, duration, pitch, velocity);

        }

        print_vector(tokens);
        line_count++;
    }

    mw.save(output_file_path);
    // std::cout << "Created: " << output_file_path << std::endl;
}

void ProjectExporter::execute(const Project& project, const std::string& output_dir) 
{
    // std::string base_dir = output_dir;
    std::string project_folder = sanitize_string(project.title);
    
    int idx = 0;
    for (const auto& track : project.tracks) {
        std::filesystem::path base_dir = output_dir;
        std::filesystem::path sub_dir = project_folder;
        std::filesystem::path output_path = base_dir / sub_dir;

        try {
            if (std::filesystem::create_directories(output_path)) {
                std::cout << "[INFO] Created directory: " << output_path.string() << std::endl;
            } else {
                std::cout << "[INFO] Directory already exists: " << output_path.string() << std::endl;
            }
        
            // Build file name:
            std::ostringstream oss;
            oss << "PROJECT_" << project_folder << "_"
                << "TRACK_" << idx << "_" 
                << sanitize_string(track.name) << ".txt";
            
            std::string output_file_name = oss.str();
            std::filesystem::path full_output_file_path = output_path / output_file_name;
            
            /* DEBUG - example of writing to output file in desired directory
            std::ofstream file(output_file_path);
            if (file.is_open()) {
                file << "Hello newly written famitracker file" << std::endl;
                std::cout << "Wrote to file: " << output_file_path.string() << std::endl;
                file.close();
            } else {
                std::cerr << "[E] Failed to open file for writing." << std::endl;
            }
            */

            export_track(track, full_output_file_path);
            idx++;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[E] Filesystem error: " << e.what() << std::endl;
        }
    }
}


