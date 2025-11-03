// container_printing.hpp

# pragma once

#include <string>
#include <sstream>

#include <vector>
#include <unordered_set>

// #include <unordered_map>
// #include <type_traits>

template <typename T>
std::string vector_to_string(const std::vector<T>& vec) {
    std::ostringstream oss;
    
    oss << "[";
    
    int i = 0;
    int n = vec.size();
    
    for (const auto& x : vec) {
        oss << x;
        if (i + 1 < n) {
            oss << ", ";
        }
        i++;
    }

    oss << "]";
    return oss.str();
}

template <typename T>
std::string uset_to_string(const std::unordered_set<T>& uset) {
    std::ostringstream oss;
    
    oss << "{";
    
    int i = 0;
    int n = uset.size();
    
    for (const auto& x : uset) {
        oss << x;
        if (i + 1 < n) {
            oss << ", ";
        }
        i++;
    }
    
    oss << "}";
    return oss.str();
}

