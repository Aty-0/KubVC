#include "editor_menu_bar.h"
#include "editor.h"

#include "editor_keyboard_window.h"
#include "editor_fps_counter_window.h"

#include "../logger.h"

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
                const auto editor = Editor::getInstance();

                if (ImGui::MenuItem("Keyboard"))
                {
                    static const auto window = editor->get<EditorKeyboardWindow>();
                    ASSERT(window != nullptr, "Couldn't get keyboard window");
                    window->setVisible(!window->isVisible());
                }

                if (ImGui::MenuItem("FPS Counter"))
                {
                    static const auto window = editor->get<EditorFpsCounterWindow>();
                    ASSERT(window != nullptr, "Couldn't get fps counter window");
                    window->setVisible(!window->isVisible());
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}