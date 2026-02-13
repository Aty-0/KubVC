#include "editor_graph_list_window.h"

namespace kubvc::editor {
    EditorGraphListWindow::EditorGraphListWindow() {
        setName("Graph List");
    }

    // Save current cursor position for expression
    std::int32_t EditorGraphListWindow::handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data) {
        if (data == nullptr || data->UserData == nullptr) {        
            return 0;
        }

        // Update cursor position 
        auto expr = *static_cast<std::shared_ptr<kubvc::math::Expression>*>(data->UserData);
        expr->setCursor(data->CursorPos);
        
        return 0;
    }

    void EditorGraphListWindow::drawGraphList(kubvc::render::GUI* gui) {
        std::int32_t expressionIndex = 0;
        for (auto expr : kubvc::math::ExpressionController::Expressions) {
            if (expr != nullptr) {
                expressionIndex++;
                drawGraphPanel(gui, expr, expr->getId(), expressionIndex);
            }
        }
    }

    void EditorGraphListWindow::drawGraphPanel(kubvc::render::GUI* gui, std::shared_ptr<kubvc::math::Expression> expr, 
        const std::int32_t& id, const std::int32_t& index) {
        static const auto fontBig = gui->getDefaultFontMathSize();
        auto& selected = kubvc::math::ExpressionController::Selected;

        ImGui::BeginGroup();

        const auto scale = ImGui::GetIO().DisplayFramebufferScale.x;
        const auto baseHeight = 55.0f;
        const auto panelHeight = baseHeight * scale;
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12 * scale, 7 * scale));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8 * scale, 8 * scale));

        ImGui::BeginChild(("##panel_" + std::to_string(id)).c_str(), ImVec2(0, panelHeight), true);
        
        const auto frameHeight = ImGui::GetFrameHeight();
        const auto verticalOffset = (panelHeight - frameHeight) * 0.5f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

        // Draw counter 
        {
            ImGui::PushFont(fontBig);
            ImGui::TextDisabled("%d:", index);
            ImGui::PopFont();
            ImGui::SameLine();
        }

        ImGui::PushFont(gui->getMathFont());
        
        if (!expr->isValid())
            ImGui::PushStyleColor(ImGuiCol_Border, INVALID_COLOR);
        else if (expr == selected)
            ImGui::PushStyleColor(ImGuiCol_Border, SELECTED_COLOR);

        const auto idStr = std::to_string(id);
                
        const auto availableWidth = ImGui::GetContentRegionAvail().x;
        const auto textBoxWidth = availableWidth * 0.5f;
        
        ImGui::SetNextItemWidth(textBoxWidth);
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
        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
        
        ImGui::PushFont(gui->getIconFont());
        
        ImGui::PushID(("##" + idStr + "_ExprRadioButton").c_str());    
        auto visible = expr->isVisible();
        ImGui::PushStyleColor(ImGuiCol_Text, visible ? 
            ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 0.7f));
        
        if (ImGui::Button(!visible ? ICON_FA_EYE_SLASH : ICON_FA_EYE)) {
            expr->setVisible(!visible);
        }
        ImGui::PopStyleColor();
        ImGui::PopID();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Change visibility for this graph.");
        }

        ImGui::SameLine();

        ImGui::PushID(("##" + idStr + "_ColorButton").c_str());
        if (ImGui::Button(ICON_FA_PALETTE)) {
            // TODO:
        }
        ImGui::PopID();

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Change graph color");
        }

        ImGui::SameLine();

        ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button(ICON_FA_TRASH)) {
            auto& exprs = kubvc::math::ExpressionController::Expressions;
            auto it = exprs.erase(std::remove_if(exprs.begin(), exprs.end(), [expr](auto it) { return it->getId() == expr->getId(); }));

            // Set as nullptr to avoid some weird behaviour
            if (it == exprs.end() && selected == expr) {
                selected.reset();
                selected = nullptr;
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopID();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Remove this graph from graph list");
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(2);
        ImGui::EndGroup();

        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::EndGroup();

        if (ImGui::IsItemClicked() && !ImGui::IsAnyItemActive()) {
            selected = expr;
        }

        ImGui::Spacing();
    }

    void EditorGraphListWindow::drawGraphListHeader() {
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

    void EditorGraphListWindow::onRender(kubvc::render::GUI* gui) {
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