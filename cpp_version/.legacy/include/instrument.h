// instrument.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

class Macro;

class BaseInst {
public:
    virtual ~BaseInst() = default;
    virtual void display();
public:
    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    std::string name;
    std::optional<Macro> macro_vol, macro_arp, macro_pit, macro_hpi, macro_dut;
};

class InstVRC7 : public BaseInst {
public:
    void display() override;
public:
    int patch;
    std::vector<int> registers;
};

class InstN163 : public BaseInst {
public:
    void display() override;
public:
    int w_size, w_pos, w_count;
    std::unordered_map<int, std::vector<int>> wave_table;
};

class InstFDS : public BaseInst {
public:
    void display() override;
public:
    bool mod_enable;
    int mod_speed, mod_depth, mod_delay;
    std::vector<int> fds_wave, fds_mod;
};
