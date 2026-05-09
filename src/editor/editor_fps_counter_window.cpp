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
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;

    EditorFpsCounterWindow::EditorFpsCounterWindow() {    
        setName("##EditorFpsCounterWindow");
        setWindowFlags(fpsCounterWindowFlags);
    }
    
    void EditorFpsCounterWindow::onRender(kubvc::render::GUI& gui) {
        static const auto taskManager = utility::TaskManager::getInstance();

        const auto io = ImGui::GetIO();
        const auto size = io.DisplaySize;
        ImGui::SetWindowPos({0, size.y - 40.0f});
        ImGui::PushFont(&gui.getDefaultFont());
        ImGui::Text("Fps:%.1f\nTask Count:%zu", io.Framerate, taskManager->size());
        ImGui::PopFont();
    }        
} 
