// constants.h

#pragma once

enum MacroType {
    VOL = 0, 
    ARP = 1, 
    PIT = 2, 
    HPI = 3, 
    DUT = 4
};

enum InstrumentFamily {
    INST_2A03 = 1,
    INST_VRC6 = 2,
    INST_VRC7 = 3,
    INST_N163 = 4,
    INST_FDS = 5,
    INST_S5B = 6
};

enum TokenType {
    NULL_TOKEN,
    NOTE_ON,
    NOISE_ON,
    NOTE_OFF,
    NOTE_RELEASE,
    NOTE_ECHO
};

enum ControlFlowType {
    SKIP_NONE = 0,  // no skip
    SKIP_BXX,       // for frame skipping
    SKIP_CXX,       // for song skipping
    SKIP_DXX        // for row skipping
};
