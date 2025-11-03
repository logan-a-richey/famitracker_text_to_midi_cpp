// container_printing.hpp

# pragma once

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <type_traits>

template <typename T>
std::string vector_to_string(const std::vector<T>& vec) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i + 1 < vec.size() ) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}
