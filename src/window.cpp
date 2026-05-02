#include "window.h"
#include "logger.h"

namespace kubvc::application {
    inline static void errorCallback(int code, const char * text) {
        KUB_ERROR("glfw error: {} {}", code, text);
    } 

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_windowHandle);
    }

    bool Window::initializeGLFW() const {
        return glfwInit();
    }
    
    void Window::destroy() {
        KUB_DEBUG("Destroy window");
        glfwTerminate();
        m_windowHandle = nullptr;
    }

    void Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, std::string_view name) {
        KUB_ASSERT(m_windowHandle == nullptr, "Window already exists!");
        KUB_ASSERT(initializeGLFW(), "GLFW initialization is failed");

        glfwSetErrorCallback(*errorCallback);

        KUB_DEBUG("Create window w:{} h:{} x:{} y:{} name:{}", w, h, x, y, name);
        m_windowHandle = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);
        if (m_windowHandle == nullptr) {
            destroy();
            KUB_FATAL("Something wrong with window creation...");
            return;
        }
        
        glfwMakeContextCurrent(m_windowHandle);
        glfwSwapInterval(0);
    }

    void Window::swapAndPool() {
        glfwSwapBuffers(m_windowHandle);
        glfwPollEvents();
    }
}