#include "editor_fps_counter_window.h"

namespace kubvc::editor
{
    EditorFpsCounterWindow::EditorFpsCounterWindow()
    {    
        static const auto fpsCounterWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking 
            | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize 
            | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse; 
        
        setWindowFlags(fpsCounterWindowFlags);
    }
    
    void EditorFpsCounterWindow::onRender(kubvc::render::GUI* gui)
    {
        auto io = ImGui::GetIO();
        ImGui::PushFont(gui->getDefaultFontMathSize());
        ImGui::Text("Fps %.1f", io.Framerate);
        ImGui::PopFont();
    }        
} 
