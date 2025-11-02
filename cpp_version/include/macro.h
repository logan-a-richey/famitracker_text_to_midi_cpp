// macro.h

#pragma once

#include <vector>

struct Macro {
    int macro_type, macro_index, macro_loop, macro_release, macro_setting;
    std::vector<int> sequence;
};
