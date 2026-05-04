#include "editor_menu_bar.h"
#include "editor.h"

#include "editor_keyboard_window.h"
#include "editor_fps_counter_window.h"

#include "../logger.h"

// TODO: relocate all save/load code
#include "../expression_controller.h"
#include "../io.h"
#include <ranges>

#include "ImGuiFileDialog.h"

namespace kubvc::editor {
    void EditorMenuBar::render(kubvc::render::GUI& gui) {
        static const auto controller = math::ExpressionController::getInstance();
        
        static const IGFD::FileDialogConfig config = { .path = "." };
        static const auto fileDialogInstance = ImGuiFileDialog::Instance();
        if (fileDialogInstance->Display("PointsSaveFileDialog")) {
            const auto filePathName = fileDialogInstance->GetFilePathName();
            if (fileDialogInstance->IsOk()) {
                const auto selected = controller->getSelected();
                if (selected != nullptr) {
                    io::FileSaver file;
                    std::vector<char> buffer; 
                    for (const auto& point : selected->getExpression().getPlotBuffer()) {
                        const auto str = std::format("{}, {}\n", point.x, point.y);
                        buffer.insert(buffer.begin(),  str.begin(), str.end());
                    }
                    KUB_ASSERT(file.save(filePathName, buffer), "failed to save file");
                }
            }

            fileDialogInstance->Close();
        }
        
        if (fileDialogInstance->Display("GraphListSaveFileDialog")) {
            if (fileDialogInstance->IsOk()) {
                const auto filePathName = fileDialogInstance->GetFilePathName();
                const auto expressions = controller->getExpressions();
                std::vector<char> fileContentBuffer;
                for (auto expression : expressions) {
                    if (expression != nullptr) {
                        const auto exprBuffer = expression->getTextBuffer().getBuffer();
                        // Find end, because we are have a fixed-size buffer    
                        const auto actualEnd = std::find(exprBuffer.begin(), exprBuffer.end(), '\0');                                                 
                        fileContentBuffer.insert(fileContentBuffer.end(), exprBuffer.begin(), actualEnd);
                        fileContentBuffer.push_back('\n');
                    }
                }
                
                io::FileSaver file;
                KUB_ASSERT(file.save(filePathName, fileContentBuffer), "failed to save file");              
            }
            fileDialogInstance->Close();
        }

        if (fileDialogInstance->Display("GraphListOpenFileDialog")) {
            if (fileDialogInstance->IsOk()) {
                const auto filePathName = fileDialogInstance->GetFilePathName();
                io::FileLoader file;
                const auto result = file.load(filePathName);
                if (result.has_value()) {
                    auto value = result.value();
                    for (const auto& str : value | std::views::split('\n') | std::views::filter([](const auto& str) { return !str.empty(); })) {
                        const auto newExpression = controller->create();
                        // Add expression to buffer
                        auto& buffer = newExpression->getTextBuffer().getBuffer();
                        buffer.insert(buffer.begin(), str.begin(), str.end());
                        // Try to parse it and evaluate 
                        newExpression->parseThenEvaluate(math::GraphLimits::GlobalLimits);
                    }
                } else {
                    KUB_ERROR("Trying to open a invalid file");
                }
            }
            fileDialogInstance->Close();
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("GraphListOpenFileDialog", "Open graph list", ".txt", config);
                }
                
                if (ImGui::MenuItem("Save Graphs(.txt)")) {
                    fileDialogInstance->OpenDialog("GraphListSaveFileDialog", "Save graph list", ".txt", config);
                }
                
                if (ImGui::MenuItem("Save graph points (.txt)")) {
                    fileDialogInstance->OpenDialog("PointsSaveFileDialog", "Save graph points", ".txt", config);
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

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}