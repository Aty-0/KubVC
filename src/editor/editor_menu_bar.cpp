#include "editor_menu_bar.h"
#include "editor.h"

#include "editor_keyboard_window.h"
#include "editor_fps_counter_window.h"

#include "../logger.h"
#include "../expression_io.h"

#include "ImGuiFileDialog.h"

namespace kubvc::editor {
    enum class FileDialogMode {
        SaveGraphs,
        SaveGraphsPoints,
        LoadGraphs,
        Unknown
    };

    void EditorMenuBar::render(kubvc::render::GUI& gui) {
        static bool showImGuiDemoWindow = false;
        static bool showImPlotDemoWindow = false;
        static const auto controller = math::ExpressionController::getInstance();
        static const auto exprIo = io::ExpressionIO::getInstance();
        static const auto fileDialogInstance = ImGuiFileDialog::Instance();
        static const IGFD::FileDialogConfig defaultFileDialogConfig = { 
            .path = ".",
            .fileName = "", 
            .filePathName = "",
            .sidePane = nullptr,
            .userFileAttributes = nullptr
        };

        static FileDialogMode fileDialogMode = FileDialogMode::Unknown;
        if (showImGuiDemoWindow) {
            ImGui::ShowDemoWindow();
        }

        if (showImPlotDemoWindow) {
            ImPlot::ShowDemoWindow();
        } 

        if (fileDialogInstance->Display("EditorMenuBarFileDialog")) {
            const auto filePathName = fileDialogInstance->GetFilePathName();
            if (fileDialogInstance->IsOk()) {
                switch (fileDialogMode)
                {
                    case FileDialogMode::LoadGraphs: {
                        exprIo->loadGraphs(filePathName);
                        break;
                    }
                    case FileDialogMode::SaveGraphs: {
                        exprIo->saveGraphs(filePathName);
                        break;
                    }
                    case FileDialogMode::SaveGraphsPoints: {
                        const auto selected = controller->getSelected();
                        exprIo->saveGraphPoints(filePathName, selected);                
                        break;
                    }            
                    default: {
                        KUB_ASSERT(false, "Unknown FileDialogMode.");
                        break;
                    }
                }
                // Reset
                fileDialogMode = FileDialogMode::Unknown;
            }
            fileDialogInstance->Close();
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Open graph list", ".txt", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::LoadGraphs;
                }
                
                if (ImGui::MenuItem("Save Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Save graph list", ".txt", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::SaveGraphs;
                }
                
                if (ImGui::MenuItem("Save graph points (.txt)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Save graph points", ".txt", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::SaveGraphsPoints;
                }
                
                if (ImGui::MenuItem("Make graph screenshot")) {
                    // TODO: 
                }
                
                ImGui::Separator();
                if (ImGui::MenuItem("Save Log file")) {
                    kubvc::utility::log->save("kub.log");
                }

                if (ImGui::MenuItem("Clear logger buffer")) {
                    kubvc::utility::log->clear();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                static const auto editor = Editor::getInstance();

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

                ImGui::Separator();
                if (ImGui::MenuItem("ImGui Demo Window")) {
                    showImGuiDemoWindow = !showImGuiDemoWindow;
                }

                if (ImGui::MenuItem("ImPlot Demo Window")) {
                    showImPlotDemoWindow = !showImPlotDemoWindow;
                }

                ImGui::EndMenu();
            }
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}