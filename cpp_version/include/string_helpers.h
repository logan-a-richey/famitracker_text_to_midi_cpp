// string_helpers.h

#pragma once

#include <string>
#include <vector>

std::string get_quote(const std::string& input_string);

std::string trim_string(const std::string& input_string);

std::string get_text_after_colon(const std::string& input_string);

std::vector<int> get_int_list(const std::string& s);

int convert_hex_str_to_int(const std::string& s);

std::vector<int> get_hex_list(const std::string& s);

std::vector<std::string> get_colon_fields(const std::string& s);

bool contains_only_spaces_and_periods(const std::string& line);

std::string sanitize_string(const std::string& input);

