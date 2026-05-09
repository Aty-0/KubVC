#include "editor_math_element_list_window.h" 
#include "expression_controller.h"

#include "../math_base.h"

namespace kubvc::editor {
    static const auto controller = math::ExpressionController::getInstance();

    EditorMathElementListWindow::EditorMathElementListWindow() {
        setName("Elements##EditorMathElementListWindow");
    }

    void EditorMathElementListWindow::addItemToExpression(std::string_view text) {
        const auto selected = controller->getSelected();
        if (selected != nullptr) {
            auto& textBuffer = selected->getTextBuffer();             
            textBuffer.insertAtCursor(text);
            controller->parseThenEvaluate(selected, math::GraphLimits::GlobalLimits);
        }
    
    } 
    void EditorMathElementListWindow::onRender(kubvc::render::GUI& gui) {
        const auto region = ImGui::GetContentRegionAvail();

        if (ImGui::BeginListBox("##EditorElementList", region)) {
            for (const auto& [name, _] : math::containers::Functions) {
                ImGui::PushFont(&gui.getMathFont());
                ImGui::Text(ICON_FA_WAVE_SQUARE);
                ImGui::PopFont();
                ImGui::SameLine();
                if (ImGui::Selectable(name.data())) {
                    addItemToExpression(name);
                } 

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Function:%s\nClick this item to insert it into the expression input field.", name.data());
                }

            }

            for (const auto& [name, _] : math::containers::Constants) {
                ImGui::PushFont(&gui.getMathFont());
                ImGui::Text(ICON_FA_BOOK);
                ImGui::PopFont();
                ImGui::SameLine();
                if (ImGui::Selectable(name.data())) {
                    addItemToExpression(name);
                } 

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Constant:%s\nClick this item to insert it into the expression input field.", name.data());
                }
            }

            ImGui::EndListBox();
        }
        
    }
}