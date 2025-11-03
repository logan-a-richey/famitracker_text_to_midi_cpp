// instrument.h

#pragma once 

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>

#include "macro.h"
#include "key_dpcm.h"
#include "constants.h"

struct FDS_Properties {
    bool mod_enable;
    int mod_speed, mod_depth, mod_delay;
    std::vector<int> fds_wave, fds_mod;
};

struct N163_Properties {
    int w_size, w_pos, w_count;
    std::unordered_map<int, std::vector<int>> wave_table;
};

struct VRC7_Properties {
    int patch;
    std::vector<int> registers;
};

struct Instrument {
    std::string display() const; 

    // Basic Properties
    InstrumentFamily family;
    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    std::string name;
    std::optional<Macro> mac_vol, mac_arp, mac_pit, mac_hpi, mac_dut;
    std::unordered_map<int, KeyDpcm> key_dpcm_notes;

    // Special Properties
    FDS_Properties fds_settings;
    N163_Properties n163_settings;
    VRC7_Properties vrc7_settings;

    // Ctor
    Instrument() = default;
    
    // Ctor2  
    Instrument( InstrumentFamily m_family, int m_index, int m_seq_vol, int m_seq_arp, int m_seq_pit, int m_seq_hpi, int m_seq_dut, const std::string& m_name) 
        : family(m_family), index(m_index), seq_vol(m_seq_vol), seq_arp(m_seq_arp), seq_pit(m_seq_pit), seq_hpi(m_seq_hpi), seq_dut(m_seq_dut), name(m_name) { /* */ }

    // Dtor
    ~Instrument() {
        mac_vol.reset();
        mac_arp.reset();
        mac_pit.reset();
        mac_hpi.reset();
        mac_dut.reset();
        
        key_dpcm_notes.clear();
        
        fds_settings.fds_wave.clear();
        fds_settings.fds_mod.clear();
        
        n163_settings.wave_table.clear();

        vrc7_settings.registers.clear();
    }
};


