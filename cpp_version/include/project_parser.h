// project_parser.h

#pragma once

#include <vector>
#include <string>

struct Project;
struct Track;

enum control_flow_t {
    SKIP_NONE = 0,  // no skip
    SKIP_BXX,       // for frame skipping
    SKIP_CXX,       // for song skipping
    SKIP_DXX        // for row skipping
};
enum token_t {
    BLANK = 0,      // ...
    NOTE_ON,        // C-2
    NOISE_ON,       // A-#
    NOTE_OFF,       // ---
    NOTE_RELEASE,   // ===
    ECHO_NOTE       // ^-2
};

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
    control_flow_t handle_control_flow(const std::string& line, const Track& track);
};
