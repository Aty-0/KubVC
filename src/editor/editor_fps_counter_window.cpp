#include "editor_fps_counter_window.h"
#include "task_manager.h"

namespace kubvc::editor {
    static const auto fpsCounterWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking 
            | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;

    EditorFpsCounterWindow::EditorFpsCounterWindow() {    
        setWindowFlags(fpsCounterWindowFlags);
    }
    
    void EditorFpsCounterWindow::onRender(kubvc::render::GUI& gui) {
        static const auto taskManager = utility::TaskManager::getInstance();

        auto io = ImGui::GetIO();
        ImGui::PushFont(gui.getDefaultFontMathSize());
        ImGui::Text("Fps %.1f\ntasks:%i", io.Framerate, taskManager->size());
        ImGui::PopFont();
    }        
} 
