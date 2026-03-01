#pragma once
#include <type_traits>
#include <functional>

namespace kubvc::utility
{
    template<typename Func, typename = std::enable_if_t<std::is_function_v<Func>>>
    class FunctionHandler {
        public:
            constexpr FunctionHandler() noexcept { }
            constexpr FunctionHandler(std::nullptr_t) noexcept : m_functor(nullptr)  { }
            constexpr FunctionHandler(Func&& func) : m_functor(std::forward<Func>(func)) { }
            constexpr FunctionHandler(const Func& func) : m_functor(func) { }        
            template<typename... Args>
            constexpr auto operator ()(Args&&... args) const {
                return std::invoke(m_functor, std::forward<Args>(args)...);
            }
                            
            constexpr bool operator()() const noexcept {
                return isEmpty();
            }
            constexpr bool isEmpty() const noexcept {
                return m_functor == nullptr;                
            }
        private:
            std::decay_t<Func> m_functor;
    };
        
    template<typename Func>
    inline constexpr bool operator==(const FunctionHandler<Func>& func,  std::nullptr_t) noexcept {
        return func.isEmpty();
    }
    
    template<typename Func>
    inline constexpr bool operator==(std::nullptr_t, const FunctionHandler<Func>& func) noexcept {
        return func.isEmpty();
    }
    
    template<typename Func>
    inline constexpr bool operator!=(const FunctionHandler<Func>& func, std::nullptr_t) noexcept {
        return !func.isEmpty();
    }
    
    template<typename Func>
    inline constexpr bool operator!=(std::nullptr_t, const FunctionHandler<Func>& func) noexcept {
        return !func.isEmpty();
    }
}