#pragma once
#include <glfw/glfw3.h>
#include "Singleton.h"
#include <cstdint>

namespace kubvc::application {
    class Window : public utility::Singleton<Window> {
        public:
            auto createWindow(std::uint32_t w, std::uint32_t h, 
                std::uint32_t x, std::uint32_t y, std::string_view name) -> void;
            
            auto destroy() -> void;
            auto swapAndPool() -> void;

            [[nodiscard]] auto shouldClose() -> bool const;
            [[nodiscard]] inline auto getHandle() -> GLFWwindow* const { return m_windowHandle; }
        private:
            [[nodiscard]] auto initializeGLFW() -> bool const; 

            GLFWwindow* m_windowHandle;
    };
}