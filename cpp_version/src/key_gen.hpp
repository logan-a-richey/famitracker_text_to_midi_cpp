// key_gen.hpp

#pragma once 

#include <string>
#include <sstream>
#include "constants.h"

std::string generate_token_key(int pattern_idx, int row_idx, int col_idx) {
    std::ostringstream oss;
    oss << "PAT=" << pattern_idx << "::";
    oss << "ROW=" << row_idx << "::";
    oss << "COL=" << col_idx;
    return oss.str();
}

std::string generate_macro_key(InstrumentFamily inst_t, MacroType macro_t, int macro_idx) {
    std::ostringstream oss;
    oss << inst_t << "::" << macro_t << "::" << macro_idx;
    return oss.str();
}

