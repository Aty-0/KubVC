#pragma once
#include <vector>
#include <string>
#include <optional>

namespace kubvc::utility::container {  
    template<typename T>
    inline static std::optional<T> get(const std::vector<std::pair<std::string_view, T>>& data,
        std::string_view name) {
            const auto result = std::find_if(data.begin(), data.end(), [name](std::pair<std::string_view, T> it) {
                return it.first == name;
            });
            if (result == data.end()) {
                return std::nullopt;
            }
            return result->second;
    }
    
    template<typename T>
    inline static bool find(const std::vector<std::pair<std::string_view, T>>& data, 
        std::string_view name) {
            return std::find_if(data.begin(), data.end(), [name](std::pair<std::string_view, T> it) {
                return it.first == name;
            }) != data.end();
    }
}