// instrument.h

#pragma once 

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>

#include "macro.h"

enum InstrumentFamily {
    INST_2A03,
    INST_VRC6,
    INST_VRC7,
    INST_N163,
    INST_FDS,
    INST_S5B
};

struct FDS_Properties {
    bool fds_mod_enable;
    int fds_mod_speed, fds_mod_depth, fds_mod_delay;
    std::vector<int> fds_wave, fds_mod;
};

struct N163_Properties {
    int n163_w_size, n163_w_pos, n163_w_count;
    std::unordered_map<int, std::vector<int>> n163_wave_table;
};

struct VRC7_Properties {
    int vrc7_patch;
    std::vector<int> vrc7_registers;
};

struct Instrument {
    std::string display() const; 

    // Basic Properties
    InstrumentFamily family;
    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    std::string name;
    std::optional<Macro> mac_vol, mac_arp, mac_pit, mac_hpi, mac_dut;
    // std::unordered_map<int, KeyDpcm> key_dpcm_notes;

    // Special Properties
    FDS_Properties fds_settings;
    N163_Properties n163_settings;
    VRC7_Properties vrc7_settings;
};


