#pragma once
#include <glfw/glfw3.h>
#include "Singleton.h"

namespace kubvc::application
{
    class Window : public utility::Singleton<Window>
    {
        public:
            void createWindow(std::uint32_t w, std::uint32_t h, 
                std::uint32_t x, std::uint32_t y, const char* name);
            
            void destroy();
            void swapAndPool();

            [[nodiscard]] bool shouldClose() const;
            [[nodiscard]] inline GLFWwindow* getHandle() const { return m_windowHandle;}
        private:
            [[nodiscard]] bool initializeGLFW() const; 

            GLFWwindow* m_windowHandle;
    };
}