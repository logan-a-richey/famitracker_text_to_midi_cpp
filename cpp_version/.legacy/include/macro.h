// macro.h

#pragma once

#include <vector>
#include <string>

class Macro {
public:
    int macro_type, macro_index, macro_loop, macro_release, macro_setting;
    std::string label;
    std::vector<int> sequence;
};
