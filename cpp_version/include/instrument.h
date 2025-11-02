// instrument.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct Macro;

struct BaseInst {

    virtual ~BaseInst() = default;
    virtual void display();

    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    std::string name;

    Macro* macro_vol; // volume offset macro
    Macro* macro_arp; // arpeggio offset macro
    Macro* macro_pit; // fine tuning
    Macro* macro_hpi; // high pitch
    Macro* macro_dut; // duty cycle
};

struct InstVRC7 : BaseInst {

    void display() override;

    int patch;
    std::vector<int> registers;
};

struct InstN163 : BaseInst {

    void display() override;

    int w_size, w_pos, w_count;
    std::unordered_map<int, std::vector<int>> wave_table;
};

struct InstFDS : BaseInst {

    void display() override;

    bool mod_enable;
    int mod_speed;
    int mod_depth;
    int mod_delay;
    std::vector<int> fds_wave;
    std::vector<int> fds_mod;
};
