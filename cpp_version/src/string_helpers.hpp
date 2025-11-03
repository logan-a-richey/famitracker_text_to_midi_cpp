// string_helpers.hpp

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <iomanip>  // for std::hex
#include <sstream>

#include "macro.h"
#include "instrument.h"

//
// Get substring between first and last double quotes
//
std::string get_quote(const std::string& input_string) {
    size_t start = input_string.find('"');
    size_t stop  = input_string.rfind('"');

    if (start == std::string::npos || stop == std::string::npos || stop <= start)
        return "";

    return input_string.substr(start + 1, stop - start - 1);
}

//
// Trim leading and trailing whitespace
//
std::string trim_string(const std::string& input_string) {
    static const std::regex pattern("^\\s+|\\s+$");
    return std::regex_replace(input_string, pattern, "");
}

//
// Get text after the first colon
//
std::string get_text_after_colon(const std::string& input_string) {
    size_t pos = input_string.find(':');
    if (pos == std::string::npos)
        return "";

    std::string result = input_string.substr(pos + 1);
    return trim_string(result);
}

//
// Convert a space-separated list of integers into std::vector<int>
//
std::vector<int> get_int_list(const std::string& s) {
    std::vector<int> values;
    std::istringstream iss(s);
    int num;
    while (iss >> num) {
        values.push_back(num);
    }
    return values;
}

//
// Convert a hexadecimal string (like "FF") into an integer
//
int convert_hex_str_to_int(const std::string& s) {
    int value = 0;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> value;
    return value;
}

//
// Convert a space-separated list of hex strings into vector<int>
// Example: "00 01 02 03" → {0, 1, 2, 3}
//
std::vector<int> get_hex_list(const std::string& s) {
    std::vector<int> values;
    std::istringstream iss(s);
    std::string token;

    while (iss >> token) {
        values.push_back(convert_hex_str_to_int(token));
    }

    return values;
}

//
// Extract all colon-separated fields after the first colon
// Example: "ROW XX : aa : bbb : cccc" → {"aa", "bbb", "cccc"}
//
std::vector<std::string> get_colon_fields(const std::string& s) {
    std::vector<std::string> fields;
    size_t pos = s.find(':');

    if (pos == std::string::npos)
        return fields; // no colon found

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
    for (char c : line) {
        if (c != ' ' && c != '.') {
            return false;
        }
    }
    return true;
}


