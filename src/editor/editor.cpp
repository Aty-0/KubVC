#include "editor.h"

#include "editor_fps_counter_window.h"
#include "editor_graph_edit_window.h"
#include "editor_graph_list_window.h"
#include "editor_keyboard_window.h"
#include "editor_plotter_window.h"
#include "editor_menu_bar.h"

namespace kubvc::editor {
    Editor::Editor() : m_windows({ 
            std::make_shared<EditorMenuBar>(), 
            std::make_shared<EditorEditGraphWindow>(), 
            std::make_shared<EditorGraphListWindow>(), 
            std::make_shared<EditorFpsCounterWindow>(), 
            std::make_shared<EditorKeyboardWindow>(), 
            std::make_shared<EditorPlotterWindow>(), 
    }) {

    }

    Editor::~Editor() {
        m_windows.clear();
    }
            
    void Editor::render(kubvc::render::GUI* gui) {
        for (auto window : m_windows) {
            window->render(gui);
        }
    }
}