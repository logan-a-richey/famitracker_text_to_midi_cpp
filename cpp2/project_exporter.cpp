// project_exporter.cpp

#include <iostream>

#include "project_exporter.h"
#include "project.h"

void ProjectExporter::execute(const Project&, const std::string& output_dir) 
{
    std::cout << "[D] Exporting project ..." << std::endl;
}
