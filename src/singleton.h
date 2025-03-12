#pragma once
#include <memory>

namespace kubvc::utility
{
    template <typename T>
    class Singleton
    {
        public:
            Singleton() { }
		    Singleton(const Singleton&) = delete;
		    Singleton(Singleton&&) = delete;
            
            Singleton& operator=(const Singleton&) = delete;
            Singleton& operator=(Singleton&&) = delete;

            [[noexcept]] [[nodiscard]] static inline T* getInstance() 
            {
                // FIXME: Possible memory leak 
                static auto instance = std::make_shared<T>();
                return instance.get();
            } 
    };
}