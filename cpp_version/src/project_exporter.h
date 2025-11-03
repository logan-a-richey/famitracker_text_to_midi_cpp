// project_exporter.h

#pragma once

#include <string>

struct Project;

class ProjectExporter {
public:
    void execute(const Project& p, const std::string& output_dir);
};

