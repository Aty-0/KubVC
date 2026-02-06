#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "gui.h"
#include "expression.h"

#include "editor/editor.h"

int main() {
    // Initialize main application components
    const auto window = kubvc::application::Window::getInstance();
    window->createWindow(1920, 1080, 0, 0, "KubVC");
    
    const auto render = kubvc::render::Renderer::getInstance();
    render->init();

    const auto gui = kubvc::render::GUI::getInstance();
    gui->init();
    
    const auto editor = kubvc::editor::Editor::getInstance();

    // Run main loop 
    while (!window->shouldClose()) {
        render->clear();
        // TODO: imgui wrap  
        gui->begin();
        gui->beginDockspace(); 
        {
            editor->render(gui);
        }
        
        gui->endDockspace();
        gui->end();
        window->swapAndPool();
    }
    
    kubvc::math::ExpressionController::Expressions.clear();
    kubvc::math::ExpressionController::Expressions.shrink_to_fit();

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
