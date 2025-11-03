// project_parser.cpp

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <iomanip>

#include "project_parser.h"
#include "project.h"
#include "track.h"
#include "string_helpers.h"
#include "key_gen.h"

// Helper function

// template <typename T>
int vector_get_next_item(const std::vector<int>& vec, int item) {
    auto it = std::find(vec.begin(), vec.end(), item);

    if (it == vec.end()) {
        std::cerr << "[E] Item not found in list." << std::endl;
        return -1;
    }

    int idx = std::distance(vec.begin(), it);
    int ndx = (idx + 1) % vec.size();

    return vec.at(ndx);
}

// *****************************************************************************
// Public Methods

//
// Main Entry Point
// Process each Track inside of Project
//
void ProjectParser::execute(Project& project) {
    std::cout << "[D] Handling project: " << project.title << std::endl;
    for (auto& track : project.tracks) {
        handle_track(project, track);
    }
}

// *****************************************************************************
// Private Methods

//
// Initialize ProjectParser
// Process a single Track from Project
//
void ProjectParser::handle_track( Project& project, Track& track ) {
    std::cout << "[D] Handling track: " << track.name << std::endl;

    if (track.orders.size() == 0) {
        std::cerr << "Cannot scan empty track" << std::endl;
        return;
    }
    
    // Init before track scanning
    // (1) store track reference, reset track lines if there are any
    track.lines.clear();
    
    // (2) store the sorted_order_keys
    sorted_order_keys.clear();
    sorted_order_keys.reserve(track.orders.size());

    std::vector<int> keys;
    keys.reserve(track.orders.size());

    for (const auto& pair : track.orders) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());
    sorted_order_keys = std::move(keys);
    
    // (3) reset target_order and target_row
    target_order = sorted_order_keys[0];
    target_row = 0;
    
    // (4) Loop through sorted_orders in track
    std::unordered_set<int> seen_it;
    while (true) {
        auto it = seen_it.find(target_order);
        if (it != seen_it.end() ){
            break;
        }
        seen_it.insert(target_order);
        handle_target_order(project, track);
    }
    std::cout << "[D] Finished parsing track: " << track.name << std::endl;
};

//
// Loop through lines in the current target order. 
// Unroll tokens and print in sequenial order
//
void ProjectParser::handle_target_order ([[maybe_unused]] Project& project, Track& track) {
    std::cout << "[D] Handling order: " << target_order << std::endl;

    std::vector<int> pattern_list = track.orders.at(target_order);

    std::vector<std::string> tokens;
    for (int i = target_row; i < track.num_rows;  ++i) {
        tokens.clear();
        for (int j = 0; j < track.num_cols; ++j) {
            // std::cout << "[D] row = " << i << " col = " << j << std::endl;

            std::string token_key = generate_token_key(pattern_list.at(j), i, j);
            auto it = track.tokens.find(token_key);

            // If token not found, create a null token:
            if (it == track.tokens.end() ) {
                std::ostringstream oss;
                oss << "... .. .";
                for (int k = 0; k < track.eff_cols.at(j); ++k) {
                    oss << " ...";
                }
                std::string null_token = oss.str();
                tokens.push_back(null_token);
                continue;
            }
            std::string retrieved_token = track.tokens.at(token_key);

            // Else continue processing the token
            
            // TODO - handle echo buffer string transformation
            // handle_echo_buffer();
            
            // TODO - regex determine token type
            // determine_token_type();

            // TODO - handle echo buffer stack
            // handle_echo_add_item(); 

            tokens.push_back(retrieved_token);
        }

        // print line to string and to stream
        std::ostringstream oss;

        // line prefix
        oss << "ORDER = "   << std::hex << std::setw(2) << std::setfill('0') << target_order << " ";
        oss << "ROW = "     << std::hex << std::setw(2) << std::setfill('0') << i << " : ";

        // print out each token, don't put ' : ' separator at the end:
        auto last_token_it = std::prev(tokens.end());
        for (auto token_it = tokens.begin(); token_it != tokens.end(); ++token_it) {
            oss << *token_it;
            if (token_it != last_token_it) {
                oss << " : ";
            }
        }
        
        std::string line = oss.str();
        std::cout << "[VERBOSE] " << line << std::endl;
        
        // TODO handle control flow
        control_flow_t res = handle_control_flow(line);
        if (res != SKIP_NONE) {
            return;
        }
    }

    target_order = vector_get_next_item(sorted_order_keys, target_order);
    target_row = 0;
}

//
// If ^-X effect, get the value, search the echo_buffers vector, and return the new string
//
std::string ProjectParser::handle_echo_buffer( 
    const std::string& token, 
    [[maybe_unused]] int col
) {
    // TODO
    return token;
}

//
// Scan for CXX, BXX, and DXX order skipping effects.
//
control_flow_t ProjectParser::handle_control_flow(
    [[maybe_unused]] const std::string& line
) {
    // TODO CXX
    if (0) {
        return SKIP_CXX;
    } 

    // TODO BXX
    if (0) {
        return SKIP_BXX;
    }

    // TODO DXX
    if (0) {
        return SKIP_DXX;
    }

    return SKIP_NONE;
}

