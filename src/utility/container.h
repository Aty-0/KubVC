#pragma once
#include <optional>
#include <initializer_list>
#include <algorithm> 

namespace kubvc::utility::container {  
    template<typename Key, typename T>
    inline static constexpr std::optional<T> get(const std::initializer_list<std::pair<Key, T>>& data, Key key) {
        const auto& result = std::ranges::find_if(data, [key](const auto& it) { return it.first == key; });
        if (result == data.end()) {
            return std::nullopt;
        }  

        return result->second;
    }
    
    template<typename Key, typename T>
    inline static constexpr bool find(const std::initializer_list<std::pair<Key, T>>& data, Key key) {
        return std::ranges::find_if(data, [key](const auto& it) { return it.first == key; }) != data.end();
    }
}