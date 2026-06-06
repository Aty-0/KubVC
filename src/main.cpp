#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "gui.h"
#include "expression.h"
#include "logger.h"

#include "editor/editor.h"
#include "editor/editor_menu_bar.h"

int main() {
    // Initialize main application components
#ifdef KUB_IS_DEBUG
    constexpr std::string_view windowName = "KubVC (Debug)";
#else 
    constexpr std::string_view windowName = "KubVC";
#endif 
    try {
        const auto window = kubvc::application::Window::getInstance();
        window->createWindow(windowName);
        
        const auto render = kubvc::render::Renderer::getInstance();
        render->init();

        const auto gui = kubvc::render::GUI::getInstance();
        gui->init();
        
        const auto editor = kubvc::editor::Editor::getInstance();
        auto menuBar = kubvc::editor::EditorMenuBar { };
        // Run main loop 
        while (!window->shouldClose()) {
            render->clear();
            gui->begin();
            gui->beginDockspaceWindow();
            {
                menuBar.render(*gui);
                gui->dockspace(); 
                {
                    editor->render(*gui);
                }
            }
            
            gui->endDockspaceWindow();
            gui->end();
            window->swapAndPool();
        }
        
        // Destroy application components
        gui->destroy();
        window->destroy();
    } catch (const std::exception& ex) {
        KUB_ERROR("fatal: {}", ex.what());
        kubvc::utility::log->save("crash_log.txt");
    } catch (const std::string& ex) {
        KUB_ERROR("fatal: {}", ex);
        kubvc::utility::log->save("crash_log.txt");
    } catch (...) {
        KUB_ERROR("fatal: unknown exception caught.");
        kubvc::utility::log->save("crash_log.txt");
    } 

    return 0;
}
