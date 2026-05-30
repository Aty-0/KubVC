#pragma once 
#include <array>
#include <atomic>
#include <concepts>
#include <utility>

namespace kubvc::utility {
    template <std::movable T> 
    class DoubleBuffer {
        public:            
            DoubleBuffer();
            DoubleBuffer(const DoubleBuffer&) = delete;
            DoubleBuffer(DoubleBuffer&&) = delete;
            auto operator=(const DoubleBuffer&) = delete; 
            auto operator=(DoubleBuffer&&) = delete;

            // Copy default value for both sides
            explicit DoubleBuffer(T defaultValue);        
            
            // Move default data 
            explicit DoubleBuffer(T front, T back);

            ~DoubleBuffer() = default;


            [[nodiscard]] T& front();
            [[nodiscard]] T& back();
            [[nodiscard]] const T& front() const;
            [[nodiscard]] const T& back() const;
            T& swap();


        private:
            std::array<T, 2> m_buffer;
            std::atomic_uint32_t m_front;
    };    

    template <std::movable T> 
    inline DoubleBuffer<T>::DoubleBuffer() : 
        m_buffer { {}, {} }, 
        m_front { 0 } {

    }

    template <std::movable T> 
    inline DoubleBuffer<T>::DoubleBuffer(T defaultValue) : 
        m_buffer { defaultValue, defaultValue }, 
        m_front { 0 } {

    }

    template <std::movable T> 
    inline DoubleBuffer<T>::DoubleBuffer(T front, T back) : 
        m_buffer { std::move(front), std::move(back) }, 
        m_front { 0 } {

    }

    template <std::movable T> 
    inline T& DoubleBuffer<T>::front() {
        return m_buffer.at(m_front.load(std::memory_order::acquire));
    }
    
    template <std::movable T> 
    inline T& DoubleBuffer<T>::back() {
        return m_buffer.at(m_front.load(std::memory_order::acquire) ^ 1);
    }

    template <std::movable T> 
    inline const T& DoubleBuffer<T>::front() const {
        return m_buffer.at(m_front.load(std::memory_order::acquire));
    }
    
    template <std::movable T> 
    inline const T& DoubleBuffer<T>::back() const {
        return m_buffer.at(m_front.load(std::memory_order::acquire) ^ 1);
    }
    
    template <std::movable T> 
    inline T& DoubleBuffer<T>::swap() {
        const auto newFront = m_front.load(std::memory_order::relaxed) ^ 1;
        m_front.store(newFront, std::memory_order::release);
        return m_buffer[newFront];
    }
}