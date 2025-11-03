#pragma once

#include <string>
#include "constants.h"

std::string generate_token_key(
    int pattern_idx, 
    int row_idx, 
    int col_idx
);

std::string generate_macro_key(
    InstrumentFamily inst_t, 
    MacroType macro_t, 
    int macro_idx
);
