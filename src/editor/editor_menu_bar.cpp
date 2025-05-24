#include "editor_menu_bar.h"

namespace kubvc::editor
{
    void EditorMenuBar::render(kubvc::render::GUI* gui)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open (.graphlist, .txt)"))
                {
                    // TODO: 
                }

                if (ImGui::MenuItem("Save (.graphlist, .txt)"))
                {
                    // TODO: 
                }

                if (ImGui::MenuItem("Make graph screenshot"))
                {
                    // TODO: 
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Keyboard"))
                {
                    // TODO: get from Editor
                    //windowKeyboardVisible = !windowKeyboardVisible;
                }

                if (ImGui::MenuItem("FPS Counter"))
                {
                    // TODO: get from Editor
                    //windowFpsVisible = !windowFpsVisible;
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}