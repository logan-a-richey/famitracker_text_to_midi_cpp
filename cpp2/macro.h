// macro.h

#pragma once 

#include <vector>
#include <string>

#include "constants.h"

struct Macro {
    InstrumentFamily inst_t;
    MacroType macro_t;
    int index, loop, release, setting;
    std::vector<int> sequence;
    
public:
    /*
    // ctor
    Macro() = delete;
    Macro( 
        InstrumentFamily inst_t_, 
        MacroType macro_t_, 
        int index_, 
        int loop_, 
        int release_, 
        int setting_, 
        std::vector<int> sequence_
    );
    */

    std::string to_str() const;
};

