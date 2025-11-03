// string_helpers.h

#pragma once 

#include <string>
#include <vector>

std::string get_quote(const std::string& input_string);
std::vector<int> get_number_field(const std::string& input_string);
std::string trim(const std::string& s);
std::vector<int> get_number_field(const std::string& input_string);
std::vector<int> get_hex_field(const std::string& input_string);
std::string generate_macro_key(const std::string& tag, int macro_type, int macro_index);
std::string generate_token_key(int pattern, int row, int col);
