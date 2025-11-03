// key_gen.hpp

#include "key_gen.h"

#include <string>
#include <sstream>
#include "constants.h"

std::string generate_token_key(int pattern_idx, int row_idx, int col_idx) {
    std::ostringstream oss;
    /*
    oss << "PAT=" << pattern_idx << "::"; oss << "ROW=" << row_idx << "::"; oss << "COL=" << col_idx;
    */
    oss << pattern_idx << "::" << row_idx << "::" << col_idx;
    return oss.str();
}

std::string generate_macro_key(InstrumentFamily inst_t, MacroType macro_t, int macro_idx) {
    std::ostringstream oss;
    /*
    oss << "INST_T=" << inst_t << "::" << "MAC_T=" << macro_t << "::" << "IDX=" << macro_idx;
    */
    oss << inst_t << "::" << macro_t << "::" << macro_idx;
    return oss.str();
}
