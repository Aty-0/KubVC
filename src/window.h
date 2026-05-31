#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include <glm/glm.hpp>

#include "singleton.h"

namespace kubvc::application {
    class Window : public utility::Singleton<Window> {
        public:
            Window() = default;
            ~Window() = default;

            void createWindow(std::string_view name);
            
            void destroy();
            void swapAndPool();
            [[nodiscard]] bool shouldClose() const;
            [[nodiscard]] GLFWwindow& getHandle() const { return *m_windowHandle; }
        private:
            void createWindow(std::uint32_t w, std::uint32_t h, 
                std::uint32_t x, std::uint32_t y, std::string_view name);

            [[nodiscard]] glm::ivec2 getMonitorResolution(); 
            [[nodiscard]] bool initializeGLFW() const; 

            GLFWwindow* m_windowHandle;
            bool m_vsync;
    };
}