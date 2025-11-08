// project_parser.h

#pragma once

#include <vector>
#include <string>
#include "constants.h"

struct Project;
struct Track;

class ProjectParser {
public:
    ProjectParser() = default;
    ~ProjectParser() {
        sorted_order_keys.clear();
    }

    void execute(Project& p);

private:
    int target_order;
    int target_row;
    std::vector<int> sorted_order_keys;
    // std::vector<EchoBuffer> echo_buffers;

private:
    void handle_track(Project& project, Track& track);
    void handle_target_order(Project& project, Track& track);
    std::string handle_echo_buffer(const std::string& token, int col);
    ControlFlowType handle_control_flow(const std::string& line, const Track& track);
};
