#pragma once
#include <glfw/glfw3.h>
#include "Singleton.h"
#include <cstdint>

namespace kubvc::application {
    class Window : public utility::Singleton<Window> {
        public:
            void createWindow(std::uint32_t w, std::uint32_t h, 
                std::uint32_t x, std::uint32_t y, std::string_view name);
            
            void destroy();
            void swapAndPool();

            bool shouldClose() const;
            inline GLFWwindow* getHandle() const { return m_windowHandle; }
        private:
            bool initializeGLFW() const; 

            GLFWwindow* m_windowHandle;
    };
}