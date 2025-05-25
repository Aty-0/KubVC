#include "editor_graph_list_window.h"

namespace kubvc::editor
{
    EditorGraphListWindow::EditorGraphListWindow()
    {
        setName("Graph List");
    }

    // Save current cursor position for expression
    std::int32_t EditorGraphListWindow::handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data)
    {
        if (data == nullptr || data->UserData == nullptr)
        {        
            return 0;
        }

        // Update cursor position 
        auto expr = *static_cast<std::shared_ptr<kubvc::math::Expression>*>(data->UserData);
        expr->setCursor(data->CursorPos);

        return 0;
    }

    void EditorGraphListWindow::drawGraphList(kubvc::render::GUI* gui)
    {
        std::int32_t expressionIndex = 0;
        for (auto expr : kubvc::math::ExpressionController::Expressions)
        {
            if (expr != nullptr)
            {
                expressionIndex++;
                drawGraphPanel(gui, expr, expr->getId(), expressionIndex);
            }
        }
    }

    void EditorGraphListWindow::drawGraphPanel(kubvc::render::GUI* gui, std::shared_ptr<kubvc::math::Expression> expr, const std::int32_t& id, const std::int32_t& index)
    {
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
                ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, EditorGraphListWindow::handleExpressionCursorPosCallback, &expr))
        {
            expr->parseAndEval(math::GraphLimits::Limits);
        }

        ImGui::PopFont();

        // Revert color changes
        auto popColor = static_cast<std::int32_t>(expr == selected || !expr->isValid());
        ImGui::PopStyleColor(popColor);

        // Set current expression by clicking on textbox 
        if (ImGui::IsItemActive() && ImGui::IsItemClicked())
        {
            selected = expr;
        }

        ImGui::SameLine();

        ImGui::PushFont(fontBig);
        ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
        if (ImGui::Button("-"))
        {
            auto& exprs = kubvc::math::ExpressionController::Expressions;
            auto it = exprs.erase(std::remove_if(exprs.begin(), exprs.end(), [expr](auto it) { return it->getId() == expr->getId(); }));

            // Set as nullptr to avoid some weird behaviour
            if (it != exprs.end() && selected == expr)
            {
                selected = nullptr;
            }
        }
        ImGui::PopID();

        ImGui::PopFont();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Remove this graph from graph list");
        }

        //static const auto fontIcon = gui->getIconFont();

        ImGui::SameLine();
        ImGui::PushFont(fontBig);
        ImGui::PushID(("##" + idStr + "_ExprRadioButton").c_str());    
        auto visible = expr->isVisible();
        if (ImGui::RadioButton("V", visible))
        {
            expr->setVisible(!visible);
        }

        ImGui::PopID();
        ImGui::PopFont();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Change visibility for this graph.");
        }
    }

    void EditorGraphListWindow::drawGraphListHeader()
    {
        auto region = ImGui::GetContentRegionAvail();

        if (ImGui::Button("Add"))
        {
            auto expr = std::make_shared<kubvc::math::Expression>();
            kubvc::math::ExpressionController::Expressions.push_back(expr);
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("A button which you can add new graph.");
        }

        ImGui::SameLine(region.x - 55.0f);
        ImGui::SetNextItemWidth(region.x - 55.0f);

        if (ImGui::Button("Clear All"))
        {
            kubvc::math::ExpressionController::Selected = nullptr;
            kubvc::math::ExpressionController::Expressions.clear();
            kubvc::math::ExpressionController::Expressions.shrink_to_fit();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Clear all your graphs.");
        }


        // TODO: Undo redo buttons

    }

    void EditorGraphListWindow::onRender(kubvc::render::GUI* gui)
    {
        const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding;


        auto windowSize = ImGui::GetWindowSize();
        if (ImGui::BeginChild("MainGraphPanel", ImVec2(windowSize.x - 15.0f, 40.0f), childFlags))
        {
            drawGraphListHeader();
        }
        ImGui::EndChild();
        
        if (ImGui::BeginChild("GraphListChild", ImVec2(windowSize.x - 15.0f, 0), childFlags, childWindowFlags))
        { 
            ImGui::TextDisabled("Graphs:");
            ImGui::Separator();
            drawGraphList(gui);
        }
        ImGui::EndChild();
    }
}