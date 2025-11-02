// string_helpers.cpp 

#include "string_helpers.h"

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <algorithm>

std::string get_quote(const std::string& input_string) {
    int start = 0;
    int stop = 0;

    for (unsigned long i = 0; i <= input_string.length(); ++i) {
        if (input_string[i] == '\"') {
            if (!start) { start = i; }
            stop = i;
        }
    }
    
    // don't include the quotes
    start++;

    // error check
    if (stop < start) { return ""; }
    int length = stop - start;
    if (length <= 0) { return ""; }

    // return substring
    return input_string.substr(start, stop - start);
}

std::string trim(const std::string& s) {
    static std::regex e("^\\s+|\\s+$");
    return std::regex_replace(s, e, "");
}

std::vector<int> get_number_field(const std::string& input_string) {
    static std::regex re_after_colon(":.*");
    static std::regex integer_regex("([+-]?\\d+)");

    std::smatch field_match;

    if (!std::regex_match(input_string, field_match, re_after_colon)) { 
        return {}; 
    }
    std::string field_match_str = field_match[1];

    std::smatch number_matches;
    auto begin = field_match_str.cbegin();
    auto end = field_match_str.cend();

    std::vector<int> numbers;
    while (std::regex_search(begin, end, number_matches, integer_regex)) {
        numbers.push_back(std::stoi(number_matches.str()));
        begin = number_matches.suffix().first; // move past the last match
    }
    return numbers;
}

// TODO test this
std::vector<int> get_hex_field(const std::string& input_string) {
    static std::regex re_after_colon(":.*");
    static std::regex integer_regex("([0-9A-F]+");

    std::smatch field_match;

    if (!std::regex_match(input_string, field_match, re_after_colon)) { 
        return {}; 
    }
    std::string field_match_str = field_match[1];

    std::smatch number_matches;
    auto begin = field_match_str.cbegin();
    auto end = field_match_str.cend();

    std::vector<int> numbers;
    while (std::regex_search(begin, end, number_matches, integer_regex)) {
        numbers.push_back(std::stoul(number_matches.str(), nullptr, 16));
        begin = number_matches.suffix().first; // move past the last match
    }
    return numbers;
}

std::string generate_macro_key(const std::string& tag, int macro_type, int macro_index) {
    std::ostringstream oss;
    oss << "INST=" << tag << "::";
    oss << "TYPE=" << macro_type << "::";
    oss << "INDEX=" << macro_index;
    return oss.str();
}

std::string generate_token_key(int pattern, int row, int col) {
    std::ostringstream oss;
    oss << "PAT=" << pattern << "::";
    oss << "ROW=" << row << "::";
    oss << "COL=" << col;
    return oss.str();
}
