#include "window.h"
#include "logger.h"

namespace kubvc::application {
    inline static void errorCallback(int code, const char * text) {
        KUB_ERROR("glfw error: {} {}", code, text);
    } 

    Window::Window() : m_windowHandle(nullptr), m_vsync(true) {

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
    
    void Window::createWindow(std::string_view name) {
        KUB_ASSERT(m_windowHandle == nullptr, "Window already exists!");
        KUB_ASSERT(initializeGLFW(), "GLFW initialization is failed");
        glfwSetErrorCallback(*errorCallback);
        
        const auto monitorRes = getMonitorResolution();
        const auto windowRes = glm::uvec2 { static_cast<std::uint32_t>(monitorRes.x / 1.5f), 
            static_cast<std::uint32_t>(monitorRes.y / 1.5f)};
        const auto windowPos = glm::uvec2 { (monitorRes.x - windowRes.x) / 2, (monitorRes.y - windowRes.y) / 2 };
        createWindow(windowRes.x, windowRes.y, windowPos.x, windowPos.y, name);
    }

    void Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, std::string_view name) {
        KUB_DEBUG("Create window w:{} h:{} x:{} y:{} name:{}", w, h, x, y, name);
        m_windowHandle = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);
        if (m_windowHandle == nullptr) {
            destroy();
            KUB_FATAL("Something wrong with window creation...");
            return;
        }
        
        glfwSetWindowPos(m_windowHandle, x, y);
        glfwMakeContextCurrent(m_windowHandle);
        setVsync(m_vsync);
    }

    glm::ivec2 Window::getMonitorResolution() {
        const auto primary = glfwGetPrimaryMonitor();
        if (primary == nullptr) {
            return { 1280, 768 };
        }
        const auto mode = glfwGetVideoMode(primary);
        
        return { mode->width, mode->height };
    }

    void Window::swapAndPool() {
        glfwSwapBuffers(m_windowHandle);
        glfwPollEvents();
    }

    void Window::setVsync(bool vsync) {
        m_vsync = vsync;
        glfwSwapInterval(static_cast<std::int32_t>(m_vsync));
    }
}