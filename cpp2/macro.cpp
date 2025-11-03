// macro.cpp

#include <iostream>
#include <string>
#include <sstream>

#include "macro.h"
#include "container_printing.hpp"

/*
// ctor
Macro::Macro(
    InstrumentFamily inst_t_, 
    MacroType macro_t_, 
    int index_, 
    int loop_, 
    int release_, 
    int setting_, 
    std::vector<int> sequence_
) {
    this->inst_t = inst_t_;
    this->macro_t = macro_t_;
    this->index = index_;
    this->loop = loop_;
    this->release = release_;
    this->setting = setting_;
    this->sequence = std::move(sequence_);
}
*/

std::string Macro::to_str() const {
    std::ostringstream oss;
    oss << "{'index': " << index;
    oss << "'sequence': " << vector_to_string(sequence) << "}";
    return oss.str();
}
