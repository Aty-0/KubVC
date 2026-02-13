#include "window.h"
#include "logger.h"

namespace kubvc::application {
    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_windowHandle);
    }

    bool Window::initializeGLFW()  const {
        return glfwInit();
    }
    
    void Window::destroy() {
        KUB_DEBUG("Destroy window");
        glfwTerminate();
        m_windowHandle = nullptr;
    }

    void Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, std::string_view name) {
        if (m_windowHandle != nullptr) {
            KUB_WARN("Window is already created!");
            return;
        }

        if (!initializeGLFW()) {
            KUB_FATAL("GLFW initialization is failed");
        }

        KUB_DEBUG("Create window w:{} h:{} x:{} y:{} name:{}", w, h, x, y, name);

        m_windowHandle = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);
        if (m_windowHandle == nullptr) {
            destroy();
            KUB_FATAL("Something wrong with window creation...");
        }

        glfwMakeContextCurrent(m_windowHandle);
        glfwSwapInterval(0);
    }

    void Window::swapAndPool() {
        glfwSwapBuffers(m_windowHandle);
        glfwPollEvents();
    }
}