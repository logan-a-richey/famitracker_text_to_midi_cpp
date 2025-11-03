// project_exporter.h

#pragma once

#include <string>

struct Project;

class ProjectExporter {
public:
    ProjectExporter() = default;
    ~ProjectExporter() = default;

    void execute(const Project& p, const std::string& output_dir);
};

