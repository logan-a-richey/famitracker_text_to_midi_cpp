// macro.h

#pragma once 

#include <vector>
#include <string>
#include <cstdint>

#include "constants.h"

struct Macro {
    InstrumentFamily inst_t;
    MacroType macro_t;
    int index, loop, release, setting;
    std::vector<int> sequence;

    // Ctor
    Macro() = default;
    
    // Ctor2
    Macro(
        InstrumentFamily m_inst_t,
        MacroType m_macro_t,
        int m_index, 
        int m_loop, 
        int m_release, 
        int m_setting,
        const std::vector<int>& m_sequence
    ) : 
         inst_t(m_inst_t),
         macro_t(m_macro_t),
         index(m_index), 
         loop(m_loop), 
         release(m_release), 
         setting(m_setting),
         sequence(m_sequence) { /* */
    }
};

