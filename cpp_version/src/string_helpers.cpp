// string_helpers.hpp

#include "string.h"

#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <iomanip>  // for std::hex
#include <sstream>

#include "macro.h"
#include "instrument.h"

std::string get_quote(const std::string& input_string) {
    /*
    Get substring between first and last double quotes
    */

    size_t start = input_string.find('"');
    size_t stop  = input_string.rfind('"');

    if (start == std::string::npos || stop == std::string::npos || stop <= start)
        return "";

    return input_string.substr(start + 1, stop - start - 1);
}

std::string trim_string(const std::string& input_string) {
    /*
    Trim leading and trailing whitespace
    */

    static const std::regex pattern("^\\s+|\\s+$");
    return std::regex_replace(input_string, pattern, "");
}

std::string get_text_after_colon(const std::string& input_string) {
    /*
    Get text after the first colon
    */

    size_t pos = input_string.find(':');
    if (pos == std::string::npos)
        return "";

    std::string result = input_string.substr(pos + 1);
    return trim_string(result);
}

std::vector<int> get_int_list(const std::string& s) {
    /*
    Convert a space-separated list of integers into std::vector<int>
    Example: "1, 2, 3" -> {1, 2, 3}
    */

    std::vector<int> values;
    std::istringstream iss(s);
    int num;
    while (iss >> num) {
        values.push_back(num);
    }
    return values;
}

int convert_hex_str_to_int(const std::string& s) {
    /*
    Convert a hexadecimal string (like "FF") into an integer
    */

    int value = 0;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> value;
    return value;
}



std::vector<int> get_hex_list(const std::string& s) {
    /*
    Convert a space-separated list of hex strings into vector<int>
    Example: "00 01 02 03" -> {0, 1, 2, 3}
    */
    
    std::vector<int> values;
    std::istringstream iss(s);
    std::string token;

    while (iss >> token) {
        values.push_back(convert_hex_str_to_int(token));
    }

    return values;
}

std::vector<std::string> get_colon_fields(const std::string& s) {
    /*
    Extract all colon-separated fields after the first colon
    Example: "ROW XX : aa : bbb : cccc" → {"aa", "bbb", "cccc"}
    */

    std::vector<std::string> fields;
    size_t pos = s.find(':');

    if (pos == std::string::npos) {
        // no colon found
        return fields; 
    }

    std::string rest = s.substr(pos + 1);
    std::istringstream iss(rest);
    std::string token;

    while (std::getline(iss, token, ':')) {
        token = trim_string(token);
        if (!token.empty()) {
            fields.push_back(token);
        }
    }

    return fields;
}

bool contains_only_spaces_and_periods(const std::string& line) {
    /*
    Returns true if the line contains only ' ' and '.'
    Used for skipping FamiTracker "null tokens," 
    which greatly reduces the number of items in the `track.tokens` unordered_map.
    */

    for (char c : line) {
        if (c != ' ' && c != '.') {
            return false;
        }
    }
    return true;
}

