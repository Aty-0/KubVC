#include "editor_menu_bar.h"
#include "editor.h"

#include "editor_keyboard_window.h"
#include "editor_fps_counter_window.h"

#include "../logger.h"

namespace kubvc::editor {
    void EditorMenuBar::render(kubvc::render::GUI& gui) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open (.graphlist, .txt)")) {
                    // TODO: 
                }

                if (ImGui::MenuItem("Save (.graphlist, .txt)")) {
                    // TODO: 
                }

                if (ImGui::MenuItem("Make graph screenshot")) {
                    // TODO: 
                }

                if (ImGui::MenuItem("Save Log file")) {
                    kubvc::utility::log->save("kub.log");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                const auto editor = Editor::getInstance();

                if (ImGui::MenuItem("Keyboard")) {
                    static const auto window = editor->get<EditorKeyboardWindow>();
                    KUB_ASSERT(window != nullptr, "Couldn't get keyboard window");
                    window->setVisible(!window->isVisible());
                }

                if (ImGui::MenuItem("FPS Counter")) {
                    static const auto window = editor->get<EditorFpsCounterWindow>();
                    KUB_ASSERT(window != nullptr, "Couldn't get fps counter window");
                    window->setVisible(!window->isVisible());
                }

                if (ImGui::BeginMenu("Themes")) {
                    if (ImGui::MenuItem("Kub Dark Theme"))        
                        gui.applyDefaultKubDarkTheme();

                    if (ImGui::MenuItem("ImGui Dark Theme"))        
                        gui.applyImGuiDarkTheme();

                    if (ImGui::MenuItem("ImGui White Theme"))        
                        gui.applyImGuiWhiteTheme();

                    if (ImGui::MenuItem("ImGui Classic Theme"))        
                        gui.applyImGuiClassicTheme();

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}