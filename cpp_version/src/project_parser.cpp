// project_parser.cpp

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <iomanip>
#include <unordered_map>

#include "project_parser.h"
#include "project.h"
#include "track.h"
#include "string_helpers.h"
#include "key_gen.h"

// Helper function

// template <typename T>
int vector_get_next_item(const std::vector<int>& vec, int item) {
    /*
        Get the next item in a vector.
        Used for getting the next order in a list of FamiTracker orders.
    */
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

void ProjectParser::execute(Project& project) {
    /*
    Main Entry Point
    Process each Track inside of Project
    */

    std::cout << "[D] Handling project: " << project.title << std::endl;
    for (auto& track : project.tracks) {
        handle_track(project, track);
    }
}

// *****************************************************************************
// Private Methods

void ProjectParser::handle_track( Project& project, Track& track ) {
    /*
    Initialize ProjectParser
    Process a single Track from Project
    */

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

void ProjectParser::handle_target_order ([[maybe_unused]] Project& project, Track& track) {
    /*   
    Loop through lines in the current target order. 
    Unroll tokens and print in sequenial order
    */

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
        
        // Handle control flow
        control_flow_t res = handle_control_flow(line, track);
        if (res != SKIP_NONE) {
            return;
        }
    }

    target_order = vector_get_next_item(sorted_order_keys, target_order);
    target_row = 0;
}

// TODO - implementation needed
std::string ProjectParser::handle_echo_buffer( const std::string& token, [[maybe_unused]] int col) {
    /* 
        If ^-X effect, get the value, search the echo_buffers vector, and return the new string.
        NOTE_ON, NOTE_NOISE, and NOTE_OFF add to echo buffer.
        NOTE_RELEASE does not add to echo buffer.
        FamiTracker echo buffer is a fixed stack of size 4. Items can be pushed off of the stack.
    */

    return token;
}

control_flow_t ProjectParser::handle_control_flow(const std::string& line, const Track& track) 
{
    /* 
        Scan for CXX, BXX, and DXX order skipping effects within a FamiTracker row.
        CXX stops the song. We simply return and the song stops. Since current target_order has not changed, seen_it will trigger and exit the while loop.
        BXX goes to order XX at row 0. If XX is not in the list of orders, we go to the last order.
        DXX goes to the next order at row XX. If XX is out of bounds, we go to the last row of the order, (num_rows - 1).
    */

    // Cxx effect - stop the song
    {
        static std::regex cxx_pattern(R"(C[0-9A-F]{2})");
        if (std::regex_search(line, cxx_pattern)) {
            return SKIP_CXX;
        }
    }

    // Bxx effect - go to order XX at row 0
    {
        static std::regex bxx_pattern(R"(B[0-9A-F]{2})");
        std::smatch matches;
        std::string::const_iterator search_start(line.cbegin());
        std::string last_match;

        // Find *all* matches and store the last
        while (std::regex_search(search_start, line.cend(), matches, bxx_pattern)) {
            last_match = matches[0];
            search_start = matches.suffix().first;
        }

        if (!last_match.empty()) {
            int bxx_value = convert_hex_str_to_int(last_match.substr(1));

            if (std::find(sorted_order_keys.begin(), sorted_order_keys.end(), bxx_value) == sorted_order_keys.end()) {
                target_order = sorted_order_keys.empty() ? 0 : sorted_order_keys.back();
            } else {
                target_order = bxx_value;
            }

            target_row = 0;
            return SKIP_BXX;
        }
    }

    // Dxx effect - go to next order, row XX
    {
        static std::regex dxx_pattern(R"(D[0-9A-F]{2})");
        std::smatch matches;
        std::string::const_iterator search_start(line.cbegin());
        std::string last_match;

        while (std::regex_search(search_start, line.cend(), matches, dxx_pattern)) {
            last_match = matches[0];
            search_start = matches.suffix().first;
        }

        if (!last_match.empty()) {
            int dxx_value = convert_hex_str_to_int(last_match.substr(1));

            // Clamp to [0, track.num_rows - 1]
            dxx_value = std::max(0, std::min(dxx_value, track.num_rows - 1));
            
            auto it = std::find(sorted_order_keys.begin(), sorted_order_keys.end(), target_order);
            if (it != sorted_order_keys.end() && ++it != sorted_order_keys.end()) {
                target_order = *it;
            } else if (!sorted_order_keys.empty()) {
                // fallback
                target_order = sorted_order_keys.back(); 
            }

            target_row = dxx_value;
            return SKIP_DXX;
        }
    }

    return SKIP_NONE;
}


















