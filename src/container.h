#pragma once
#include <string>
#include <optional>
#include <initializer_list>

namespace kubvc::utility::container {  
    template<typename T>
    inline static constexpr std::optional<T> get(const std::initializer_list<std::pair<std::string_view, T>>& data,
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
    inline static constexpr bool find(const std::initializer_list<std::pair<std::string_view, T>>& data, 
        std::string_view name) {
            return std::find_if(data.begin(), data.end(), [name](std::pair<std::string_view, T> it) {
                return it.first == name;
            }) != data.end();
    }
}