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


void ProjectExporter::export_track(const Track& track, const std::string& full_output_path)
{
    // MidiWriter midi;
    
    for (const auto& line : track.lines)
    {
        /* process track here */
    }

    // midi.save(full_output_path);
    std::cout << "Created: " << full_output_path << std::endl;
}

void ProjectExporter::execute(const Project& project, const std::string& output_dir) 
{
    // std::string base_dir = output_dir;
    std::string project_folder = sanitize_string(project.title);
    
    int idx = 0;
    for (const auto& track : project.tracks)
    {
        std::filesystem::path base_dir = output_dir;
        std::filesystem::path sub_dir = project_folder;
        std::filesystem::path output_path = base_dir / sub_dir;

        try 
        {
            if (std::filesystem::create_directories(output_path)) 
            {
                std::cout << "Created directory: " << output_path.string() << std::endl;
            } 
            else 
            {
                std::cout << "Directory already exists: " << output_path.string() << std::endl;
            }
        
            // Build file name:
            std::ostringstream oss;
            oss << "PROJECT_" << project_folder << "_"
                << "TRACK_" << idx << "_" 
                << sanitize_string(track.name) << ".txt";
            
            std::string output_file_name = oss.str();
            std::filesystem::path output_file_path = output_path / output_file_name;

            std::ofstream file(output_file_path);
            if (file.is_open()) 
            {
                file << "Hello newly written famitracker file" << std::endl;
                std::cout << "Wrote to file: " << output_file_path.string() << std::endl;
                file.close();
            } 
            else 
            {
                std::cerr << "[E] Failed to open file for writing." << std::endl;
            }
            
            // export_track(track, full_output_path)
            idx++;
        }
        catch (const std::filesystem::filesystem_error& e) 
        {
            std::cerr << "[E] Filesystem error: " << e.what() << std::endl;
        }
    }
}


