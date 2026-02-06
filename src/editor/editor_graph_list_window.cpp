#include "editor_graph_list_window.h"

namespace kubvc::editor {
    EditorGraphListWindow::EditorGraphListWindow() {
        setName("Graph List");
    }

    // Save current cursor position for expression
    auto EditorGraphListWindow::handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data) -> std::int32_t {
        if (data == nullptr || data->UserData == nullptr) {        
            return 0;
        }

        // Update cursor position 
        auto expr = *static_cast<std::shared_ptr<kubvc::math::Expression>*>(data->UserData);
        expr->setCursor(data->CursorPos);
        
        return 0;
    }

    auto EditorGraphListWindow::drawGraphList(kubvc::render::GUI* gui) -> void {
        std::int32_t expressionIndex = 0;
        for (auto expr : kubvc::math::ExpressionController::Expressions) {
            if (expr != nullptr) {
                expressionIndex++;
                drawGraphPanel(gui, expr, expr->getId(), expressionIndex);
            }
        }
    }

    auto EditorGraphListWindow::drawGraphPanel(kubvc::render::GUI* gui, std::shared_ptr<kubvc::math::Expression> expr, 
        const std::int32_t& id, const std::int32_t& index) -> void {
        static const auto fontBig = gui->getDefaultFontMathSize();
        auto& selected = kubvc::math::ExpressionController::Selected;

        // Draw counter 
        ImGui::PushFont(fontBig);
        ImGui::TextDisabled("%d:", index);
        ImGui::PopFont();

        ImGui::SameLine();

        ImGui::PushFont(gui->getMathFont());
        // Set special color for textbox border when we are selected expression or get invalid node somewhere kekw
        if (!expr->isValid())
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, INVALID_COLOR);
        else if (expr == selected)
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, SELECTED_COLOR);

        const auto idStr = std::to_string(id);

        if (ImGui::InputText(("##" + idStr + "_ExprInputText").c_str(), expr->getTextBuffer().data(), expr->getTextBuffer().size(), 
                ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, 
                EditorGraphListWindow::handleExpressionCursorPosCallback, &expr)) {
            expr->parseThenEval(math::GraphLimits::Limits);
        }

        ImGui::PopFont();

        // Revert color changes
        auto popColor = static_cast<std::int32_t>(expr == selected || !expr->isValid());
        ImGui::PopStyleColor(popColor);

        // Set current expression by clicking on textbox 
        if (ImGui::IsItemActive() && ImGui::IsItemClicked()) {
            selected = expr;
        }

        ImGui::SameLine();

        ImGui::PushFont(fontBig);
        ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
        if (ImGui::Button("-")) {
            auto& exprs = kubvc::math::ExpressionController::Expressions;
            auto it = exprs.erase(std::remove_if(exprs.begin(), exprs.end(), [expr](auto it) { return it->getId() == expr->getId(); }));

            // Set as nullptr to avoid some weird behaviour
            if (it != exprs.end() && selected == expr) {
                selected.reset();
                selected = nullptr;
            }
        }
        ImGui::PopID();

        ImGui::PopFont();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Remove this graph from graph list");
        }

        //static const auto fontIcon = gui->getIconFont();

        ImGui::SameLine();
        ImGui::PushFont(fontBig);
        ImGui::PushID(("##" + idStr + "_ExprRadioButton").c_str());    
        auto visible = expr->isVisible();
        if (ImGui::RadioButton("V", visible)) {
            expr->setVisible(!visible);
        }

        ImGui::PopID();
        ImGui::PopFont();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Change visibility for this graph.");
        }
    }

    auto EditorGraphListWindow::drawGraphListHeader() -> void {
        auto region = ImGui::GetContentRegionAvail();

        const auto buttonSize = ImVec2(0, region.y);
        if (ImGui::Button("Add", buttonSize)) {
            auto expr = std::make_shared<kubvc::math::Expression>();
            kubvc::math::ExpressionController::Expressions.push_back(expr);
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("A button which you can add new graph.");
        }

        ImGui::SameLine(region.x - 69.0f);
        ImGui::SetNextItemWidth(region.x - 55.0f);

        if (ImGui::Button("Clear All", buttonSize)) {
            kubvc::math::ExpressionController::Selected = nullptr;
            kubvc::math::ExpressionController::Expressions.clear();
            kubvc::math::ExpressionController::Expressions.shrink_to_fit();
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Clear all your graphs.");
        }
        // TODO: Undo redo buttons
    }

    auto EditorGraphListWindow::onRender(kubvc::render::GUI* gui) -> void {
        const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding;

        auto windowSize = ImGui::GetWindowSize();
        if (ImGui::BeginChild("GraphListHeader", ImVec2(windowSize.x, 42.0f), childFlags)) {
            drawGraphListHeader();
        }
        ImGui::EndChild();
        
        if (ImGui::BeginChild("GraphListChild", ImVec2(windowSize.x, 0), childFlags, childWindowFlags)) { 
            ImGui::TextDisabled("Graphs:");
            ImGui::Separator();
            drawGraphList(gui);
        }
        ImGui::EndChild();
    }
}