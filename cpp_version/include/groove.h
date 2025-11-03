// groove.h

#pragma once

#include <vector>

struct Groove {
    int index;
    int size;
    std::vector<int> sequence;

    // Ctor
    Groove() = default;

    // Ctor2
    Groove( int m_index, int m_size, const std::vector<int>& m_seq) 
        : index(m_index), size(m_size), sequence(m_seq) { /* */ }

    // Dtor
    ~Groove() {
        sequence.clear();
    }
};
