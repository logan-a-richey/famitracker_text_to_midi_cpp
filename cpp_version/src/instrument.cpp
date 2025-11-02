// instrument.cpp

#include "instrument.h"
#include "string_helpers.h"
#include "utils.hpp"

#include <iostream>

void BaseInst::display() {
    std::cout << "{\'index\': " << index << ", ";
    std::cout << "\'seq_vol\': " << seq_vol << ", ";
    std::cout << "\'seq_arp\': " << seq_arp << ", ";
    std::cout << "\'seq_pit\': " << seq_pit << ", ";
    std::cout << "\'seq_hpi\': " << seq_hpi << ", ";
    std::cout << "\'seq_dut\': " << seq_dut << ", ";
    std::cout << "\'name\': \'" << name << "\'}";
}

void InstN163::display() {
    std::cout << "{\'index\': " << index << ", ";
    std::cout << "\'seq_vol\': " << seq_vol << ", ";
    std::cout << "\'seq_arp\': " << seq_arp << ", ";
    std::cout << "\'seq_pit\': " << seq_pit << ", ";
    std::cout << "\'seq_hpi\': " << seq_hpi << ", ";
    std::cout << "\'seq_dut\': " << seq_dut << ", ";
    std::cout << "\'w_size\': " << w_size << ", ";
    std::cout << "\'w_pos\': " << w_pos << ", ";
    std::cout << "\'w_count\': " << w_count << ", ";
    std::cout << "\'name\': \'" << name << "\'}";
}

void InstFDS::display() {
    std::cout << "{\'index\': " << index << ", ";
    std::cout << "\'seq_vol\': " << seq_vol << ", ";
    std::cout << "\'seq_arp\': " << seq_arp << ", ";
    std::cout << "\'seq_pit\': " << seq_pit << ", ";
    std::cout << "\'seq_hpi\': " << seq_hpi << ", ";
    std::cout << "\'seq_dut\': " << seq_dut << ", ";
    std::cout << "\'mod_enable\': " << mod_enable << ", ";
    std::cout << "\'mod_speed\': " << mod_speed << ", ";
    std::cout << "\'mod_depth\': " << mod_depth << ", ";
    std::cout << "\'mod_delay\': " << mod_delay << ", ";
    std::cout << "\'fds_wave\': " << vector_to_string(fds_wave) << ", ";
    std::cout << "\'fds_mod\': " << vector_to_string(fds_mod) << ", ";
    std::cout << "\'name\': \'" << name << "\'}";
}

void InstVRC7::display() {
    std::cout << "{\'index\': " << index << ", ";
    /*
    std::cout << "\'seq_vol\': " << seq_vol << ", ";
    std::cout << "\'seq_arp\': " << seq_arp << ", ";
    std::cout << "\'seq_pit\': " << seq_pit << ", ";
    std::cout << "\'seq_hpi\': " << seq_hpi << ", ";
    std::cout << "\'seq_dut\': " << seq_dut << ", ";
    */
    std::cout << "\'patch\': " << patch << ", ";
    std::cout << "\'registers\': " << vector_to_string(registers) << ", ";
    std::cout << "\'name\': \'" << name << "\'}";
}

