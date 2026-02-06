#include "window.h"
#include "logger.h"

namespace kubvc::application {
    auto Window::shouldClose() -> bool const {
        return glfwWindowShouldClose(m_windowHandle);
    }

    auto Window::initializeGLFW() -> bool const {
        return glfwInit();
    }
    
    auto Window::destroy() -> void {
        KUB_DEBUG("Destroy window");
        glfwTerminate();
        m_windowHandle = nullptr;
    }

    auto Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, std::string_view name) -> void {
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

    auto Window::swapAndPool() -> void {
        glfwSwapBuffers(m_windowHandle);
        glfwPollEvents();
    }
}