// utils.hpp 

#pragma once
#include <sstream>
#include <vector>
#include <string>
#include <type_traits>

template<typename T>
std::string vector_to_string(const std::vector<T>& vec){
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if constexpr (std::is_same_v<T, std::string>) { 
            oss << "\'" << vec[i] << "\'";
        } else { 
            oss << vec[i]; 
        }
        if (i + 1 < vec.size() ) { 
            oss << ", "; 
        }
    }
    oss << "]";
    return oss.str();
}
