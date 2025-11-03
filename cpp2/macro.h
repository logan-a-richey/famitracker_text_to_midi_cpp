// macro.h

#pragma once 

#include <vector>

enum MacroFamily {
    MACRO_2A03,
    MACRO_VRC6,
    MACRO_VRC7,
    MACRO_N163,
    MACRO_FDS,
    MACRO_S5B
};

struct Macro {
    MacroFamily family;
    int type, index, loop, release, setting;
    std::vector<int> sequence;
};
