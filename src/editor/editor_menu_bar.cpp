#include "editor_menu_bar.h"
#include "editor.h"

#include "editor_keyboard_window.h"
#include "editor_fps_counter_window.h"

#include "../logger.h"
#include "../expression_io.h"
#include "../application_config.h"
#include "../macro_controller.h"
#include "../window.h"

#include "ImGuiFileDialog.h"

namespace kubvc::editor {
    enum class FileDialogMode {
        SaveGraphs,
        SaveGraphsPoints,
        SaveMacros,
        LoadGraphs,
        LoadMacros,
        Unknown
    };

    void EditorMenuBar::render(kubvc::render::GUI& gui) {
        static bool showImGuiDemoWindow = false;
        static bool showImPlotDemoWindow = false;
        static const auto macroController = algorithm::MacroController::getInstance();
        static const auto controller = math::ExpressionController::getInstance();
        static const auto exprIo = io::ExpressionIO::getInstance();
        static const auto fileDialogInstance = ImGuiFileDialog::Instance();
        static const auto editor = Editor::getInstance();

        static const IGFD::FileDialogConfig defaultFileDialogConfig = { 
            .path = ".",
            .fileName = "", 
            .filePathName = "",
            .sidePane = nullptr,
            .userFileAttributes = nullptr
        };

        static FileDialogMode fileDialogMode = FileDialogMode::Unknown;
        if (showImGuiDemoWindow) {
            ImGui::ShowDemoWindow(&showImGuiDemoWindow);
        }

        if (showImPlotDemoWindow) {
            ImPlot::ShowDemoWindow(&showImPlotDemoWindow);
        } 

        if (fileDialogInstance->Display("EditorMenuBarFileDialog")) {
            const auto filePathName = fileDialogInstance->GetFilePathName();
            if (fileDialogInstance->IsOk()) {
                switch (fileDialogMode)
                {
                    case FileDialogMode::LoadMacros: {
                        macroController->load(filePathName);
                        break;
                    }
                    case FileDialogMode::SaveMacros: {
                        macroController->save(filePathName);
                        break;
                    }
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

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New project")) {
                    // TODO: 
                }

                if (ImGui::MenuItem("Open project")) {
                    // TODO: 
                }

                if (ImGui::MenuItem("Save project")) {
                    // TODO: 
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Open Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Open graph list", ".txt", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::LoadGraphs;
                }
                
                if (ImGui::MenuItem("Save Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Save graph list", ".txt", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::SaveGraphs;
                }
                
                
                ImGui::Separator();
                if (ImGui::MenuItem("Load macros (.macro)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Load macros", ".macro", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::LoadMacros;
                }

                if (ImGui::MenuItem("Save macros (.macro)")) {
                    fileDialogInstance->OpenDialog("EditorMenuBarFileDialog", "Save macros", ".macro", defaultFileDialogConfig);
                    fileDialogMode = FileDialogMode::SaveMacros;
                }

                ImGui::Separator();
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

            if (ImGui::BeginMenu("Mode")) {
                static const auto config = application::ApplicationConfig::getInstance();

                auto& iconFont = gui.getIconFont();
                
                if (config->getMode() == application::MathMode::Real) {
                    ImGui::PushFont(&iconFont);
                    ImGui::Text(ICON_FA_CHECK);
                    ImGui::PopFont();
                    ImGui::SameLine();
                }

                if (ImGui::MenuItem("Real")) {
                    config->setMode(application::MathMode::Real);
                    controller->reevaluateAllExpressions(math::GraphLimits::GlobalLimits);
                }
                if (config->getMode() == application::MathMode::Complex) {
                    ImGui::PushFont(&iconFont);
                    ImGui::Text(ICON_FA_CHECK);
                    ImGui::PopFont();
                    ImGui::SameLine();
                }

                if (ImGui::MenuItem("Complex")) {                    
                    config->setMode(application::MathMode::Complex);
                    controller->reevaluateAllExpressions(math::GraphLimits::GlobalLimits);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Other")) {
                static const auto window = application::Window::getInstance();
                const auto vsync = window->getVsync();

                if (ImGui::MenuItem(vsync ? "Disable Vsync" : "Enable Vsync")) {                    
                    window->setVsync(!vsync);    
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
        if (ImGui::BeginChild("##Toolbar", ImVec2(0, ImGui::GetFrameHeight()), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImGui::Dummy({ 5.0f, 0.0f });
            ImGui::SameLine();
            editor->renderMenuBarButtons(gui);
        }
        ImGui::EndChild();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}