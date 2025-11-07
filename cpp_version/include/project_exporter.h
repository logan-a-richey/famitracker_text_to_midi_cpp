// project_exporter.h

#pragma once

#include <string>

struct Track;
struct Project;

class ProjectExporter {
public:
    ProjectExporter() = default;
    ~ProjectExporter() = default;

    void execute(const Project& p, const std::string& output_dir);

private:
    void export_track(const Track& track, const std::string& full_output_path);

};

