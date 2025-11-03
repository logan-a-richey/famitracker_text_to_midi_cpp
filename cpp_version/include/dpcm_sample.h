// dpcm_sample.h 

#pragma once

#include <string>
#include <vector>

struct DpcmSample {
    int index;
    int size;
    std::string name;
  
    std::vector<int> data;
    
public:
    // Ctor
    DpcmSample() = default;

    // Ctor2
    DpcmSample(int a, int b, const std::string& c) : index(a), size(b), name(c) { /* */ }

    // Dtor
    ~DpcmSample() {
        data.clear();
    }
};
