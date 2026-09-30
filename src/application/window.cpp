#include "window.h"
#include <iterator>
#include "../utility/logger.h"

#if !defined(_WIN32)
    // On Windows the icon comes from the GLFW_ICON resource (see resources/app.rc)
    #include "../resources/app_icon_data.h"
#endif

namespace kubvc::application {
    inline static void errorCallback(std::int32_t code, const char * text) {
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
        // Initialize glfw and check on window exist  
        KUB_ASSERT(m_windowHandle == nullptr, "Window already exists!");
        KUB_ASSERT(initializeGLFW(), "GLFW initialization is failed");

        // Add error callbacks
        glfwSetErrorCallback(*errorCallback);
        
        // Make overage resoulution   
        const auto monitorRes = getMonitorResolution();
        const auto windowRes = glm::uvec2 { static_cast<std::uint32_t>(monitorRes.x / 1.5f), 
            static_cast<std::uint32_t>(monitorRes.y / 1.5f)};
        const auto windowPos = glm::uvec2 { (monitorRes.x - windowRes.x) / 2, (monitorRes.y - windowRes.y) / 2 };

        // Create window by overage resolution and position
        createWindow(windowRes.x, windowRes.y, windowPos.x, windowPos.y, name);
    }

    void Window::createWindow(std::uint32_t w, std::uint32_t h, std::uint32_t x, std::uint32_t y, std::string_view name) {
        KUB_DEBUG("Create window w:{} h:{} x:{} y:{} name:{}", w, h, x, y, name);
        applyWindowHints();
        m_windowHandle = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);
        if (m_windowHandle == nullptr) {
            destroy();
            KUB_FATAL("Something wrong with window creation...");
            return;
        }
        
        setWindowIcon();
        glfwSetWindowPos(m_windowHandle, x, y);
        glfwMakeContextCurrent(m_windowHandle);
        setVsync(m_vsync);
    }

    void Window::applyWindowHints() const {
        // Identifiers must match resources/linux/kubvc.desktop
        // (StartupWMClass for X11, <app_id>.desktop file name for Wayland),
        // otherwise the desktop can't associate the window with its icon.
#ifdef GLFW_X11_CLASS_NAME
        glfwWindowHintString(GLFW_X11_CLASS_NAME, "kubvc");
        glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "kubvc");
#endif
#ifdef GLFW_WAYLAND_APP_ID
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, "kubvc");
#endif
    }

    void Window::setWindowIcon() {
#if defined(_WIN32)
        // Nothing to do: GLFW loads the "GLFW_ICON" resource from app.rc by itself
#else
    #if defined(GLFW_PLATFORM_WAYLAND)
        // Wayland has no per-window icon API; the icon is taken from the .desktop file by app_id
        if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
            return;
        }
    #endif
        const GLFWimage images[] = {
            { 16,  16,  const_cast<unsigned char*>(icon_data::k16x16) },
            { 32,  32,  const_cast<unsigned char*>(icon_data::k32x32) },
            { 48,  48,  const_cast<unsigned char*>(icon_data::k48x48) },
            { 64,  64,  const_cast<unsigned char*>(icon_data::k64x64) },
            { 128, 128, const_cast<unsigned char*>(icon_data::k128x128) },
        };
        glfwSetWindowIcon(m_windowHandle, static_cast<int>(std::size(images)), images);
#endif
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