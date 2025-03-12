#include "window.h"

#include "logger.h"

namespace kubvc::application
{
    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_windowHandle);
    }

    bool Window::initializeGLFW() const
    {
        return glfwInit();
    }
    
    void Window::destroy()
    {
        DEBUG("Destroy window");
        glfwTerminate();
        m_windowHandle = nullptr;
    }

    void Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, const char* name)
    {
        if (m_windowHandle != nullptr)
        {
            DEBUG("Window is already created!");
            return;
        }

        if (!initializeGLFW())
        {
            FATAL("GLFW initialization is failed");
        }

        DEBUG("Create window w:%d h:%d x:%d y:%d name:%s", w, h, x, y, name);

        m_windowHandle = glfwCreateWindow(w, h, name, nullptr, nullptr);
        if (m_windowHandle == nullptr)
        {
            destroy();
            FATAL("Something wrong with window creation...");
        }
        
        glfwMakeContextCurrent(m_windowHandle);
    }

    void Window::swapAndPool()
    {
        glfwSwapBuffers(m_windowHandle);
        glfwPollEvents();
    }
}