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
            auto& buffer = textBuffer.getBuffer();
             
            const auto& beg = buffer.begin() + textBuffer.getCursor();            
            buffer.insert(beg, text.begin(), text.end());
            
            textBuffer.setCursor(textBuffer.getCursor() + text.size());        
            controller->parseThenEvaluate(selected, math::GraphLimits::GlobalLimits);
        }
    
    } 
    void EditorMathElementListWindow::onRender([[maybe_unused]] kubvc::render::GUI& gui) {
        const auto region = ImGui::GetContentRegionAvail();

        if (ImGui::BeginListBox("##EditorElementList", region)) {
            for (const auto& [name, _] : math::containers::Functions) {
                if (ImGui::Selectable(name.data())) {
                    addItemToExpression(name);
                } 
            }

            for (const auto& [name, _] : math::containers::Constants) {
                if (ImGui::Selectable(name.data())) {
                    addItemToExpression(name);
                } 
            }

            ImGui::EndListBox();
        }
        
    }
}