#pragma once
#include <memory>

namespace kubvc::utility {
    template <typename T>
    class Singleton {
        public:
            Singleton() { }
		    Singleton(const Singleton&) = delete;
		    Singleton(Singleton&&) = delete;
            
            Singleton& operator=(const Singleton&) = delete;
            Singleton& operator=(Singleton&&) = delete;

            [[nodiscard]] static inline auto getInstance() -> T* {
                static auto instance = T();
                return &instance;
            } 
    };
}