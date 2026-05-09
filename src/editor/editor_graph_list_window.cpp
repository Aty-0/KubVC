#include "editor_graph_list_window.h"
#include "expression_controller.h"

namespace kubvc::editor {
    static const auto controller = kubvc::math::ExpressionController::getInstance();
    
    EditorGraphListWindow::EditorGraphListWindow() {
        setName("Graph List##EditorGraphListWindow");
    }

    std::int32_t EditorGraphListWindow::handleTextBoxMultilineInput(ImGuiInputTextCallbackData* data) {
        if (data == nullptr || data->UserData == nullptr) {        
            return 0;
        }

        if (data->EventFlag == ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter) {
            if (data->EventChar == '\n' || data->EventChar == '\r') {
                return 1; 
            }
        }

        return handleExpressionCursorPosCallback(data);
    }


    // Save current cursor position for expression
    std::int32_t EditorGraphListWindow::handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data) {
        if (data == nullptr || data->UserData == nullptr) {        
            return 0;
        }

        // Update cursor position 
        const auto expr = static_cast<kubvc::math::ExpressionTextBuffer*>(data->UserData);
        expr->setCursor(data->CursorPos);        
        
        return 0;
    }

    void EditorGraphListWindow::drawGraphList(kubvc::render::GUI& gui) {
        ImGuiListClipper clipper{ };
        const auto expressions = controller->getExpressions();
        clipper.Begin(static_cast<std::int32_t>(expressions.size()), 
            ImGui::GetTextLineHeightWithSpacing());
        while (clipper.Step()) {
            for (std::int32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                const auto model = expressions[i];
                drawGraphPanel(gui, model, i);
            }
        }
    }

    void EditorGraphListWindow::drawGraphPanel(kubvc::render::GUI& gui, std::shared_ptr<math::ExpressionModel> model, std::int32_t index) {
        static auto& fontBig = gui.getDefaultFontMathSize();        

        const auto selectedModel = controller->getSelected();
        
        auto& currentExpression = model->getExpression();
        auto& currentSettings = model->getSettings();
        
        // Is text box expanded
        const auto expandTextBox = currentSettings.getExpandTextBox();

        // id stuff
        const auto currentModelId = model->getId();
        const auto idStr = std::to_string(currentModelId);
        const auto currentExpressionIsSelected = selectedModel == nullptr ? false : currentModelId == selectedModel->getId();

        ImGui::BeginGroup();
        
        const auto frameHeight = ImGui::GetFrameHeight();
        const auto style = ImGui::GetStyle();
        const auto scale = ImGui::GetIO().DisplayFramebufferScale.x;
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12 * scale, 7 * scale));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8 * scale, 8 * scale));

        const auto height = expandTextBox ? frameHeight * 3.5f : frameHeight * 2.5f;
        const auto padding = style.FramePadding.y * 2 + style.ItemSpacing.y;
        const auto totalHeight = height + padding;
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, 0); 
        ImGui::BeginChild(("##graphPanel" + idStr).c_str(), ImVec2(0, totalHeight), true);
        ImGui::PopStyleColor();
        
        // Draw current index text 
        {
            ImGui::PushFont(&fontBig);
            ImGui::TextDisabled("%d", index);
            ImGui::PopFont();
            ImGui::SameLine();
        }

        ImGui::SameLine();

        // Draw icons
        ImGui::BeginGroup();
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));

            ImGui::PushFont(&gui.getIconFont());
            {    
                // Draw expand text box icon  
                {
                    ImGui::PushID(("##" + idStr + "ExpandTextBoxButton").c_str());                        
                    if (ImGui::Button(!expandTextBox ? ICON_FA_EXPAND : ICON_FA_COMPRESS)) {
                        currentSettings.setExpandTextBox(!expandTextBox);
                    }
                    ImGui::PopID();

                    ImGui::PushFont(&gui.getDefaultFont());
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Expand text box");
                    }
                    ImGui::PopFont();
                }
                ImGui::SameLine();
                // Draw visibility icon  
                {
                    ImGui::PushID(("##" + idStr + "_ExprRadioButton").c_str());    
                    const auto visible = currentSettings.getVisible();
                    ImGui::PushStyleColor(ImGuiCol_Text, visible ? 
                        ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 0.7f));
                    
                    if (ImGui::Button(!visible ? ICON_FA_EYE_SLASH : ICON_FA_EYE)) {
                        currentSettings.setVisible(!visible);
                    }
                    ImGui::PopStyleColor();
                    ImGui::PopID();

                    ImGui::PushFont(&gui.getDefaultFont());
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Change visibility for this graph.");
                    }
                    ImGui::PopFont();
                }
                ImGui::SameLine();
                // Draw change color icon 
                {
                    ImGui::PushID(("##" + idStr + "_ColorButton").c_str());
                    if (ImGui::Button(ICON_FA_PALETTE)) {
                        // TODO:
                    }
                    ImGui::PopID();
                    
                    ImGui::PushFont(&gui.getDefaultFont());
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Change graph color");
                    }
                    ImGui::PopFont();
                } 
                ImGui::SameLine();
                // Draw remove icon 
                {
                    ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                    if (ImGui::Button(ICON_FA_TRASH)) {
                        const auto result = controller->removeById(currentModelId);
                        if (result && selectedModel == model) {
                            controller->resetSelected();
                        }
                    }
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                    
                    ImGui::PushFont(&gui.getDefaultFont());
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Remove this graph from graph list");
                    }
                    ImGui::PopFont();
                }
                ImGui::SameLine();
                // Draw error hint 
                {
                    const auto lastErrorMessage = currentExpression.getLastErrorMessage();
                    if (!currentExpression.isValid() && !lastErrorMessage.empty()) {
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

                        ImGui::Text(ICON_FA_TRIANGLE_EXCLAMATION);
                        ImGui::PopStyleColor();

                        ImGui::PushFont(&gui.getDefaultFont());
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip(lastErrorMessage.c_str());
                        }
                        ImGui::PopFont();
                    }
                }      
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(2);
        }
        ImGui::EndGroup();

        // Draw textbox 
        {
            ImGui::PushFont(&gui.getMathFont());
            // FIXME: for some reason not work
            if (!currentExpression.isValid())
                ImGui::PushStyleColor(ImGuiCol_Border, INVALID_COLOR);
            else if (currentExpressionIsSelected)
                ImGui::PushStyleColor(ImGuiCol_Border, SELECTED_COLOR);

                    
            
            const auto textBoxWidth = ImGui::GetContentRegionAvail().x;
            auto& currentExpressionTextBuffer = model->getTextBuffer(); 
            auto& textBuffer = currentExpressionTextBuffer.getBuffer();
            bool textChanged = false;
            
            if (expandTextBox) {
                constexpr auto textBoxMultilineFlags = ImGuiInputTextFlags_::ImGuiInputTextFlags_WordWrap 
                    | ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank 
                    | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways 
                    | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter;

                textChanged = ImGui::InputTextMultiline(
                    ("##" + idStr + "_ExprInputText").c_str(),
                    textBuffer.data(),
                    textBuffer.size(),
                    ImVec2(textBoxWidth, frameHeight * 2),
                    textBoxMultilineFlags,
                    EditorGraphListWindow::handleTextBoxMultilineInput,
                    &currentExpressionTextBuffer
                );
            } else {
                ImGui::SetNextItemWidth(textBoxWidth);
                textChanged = ImGui::InputText(
                    ("##" + idStr + "_ExprInputText").c_str(), 
                    textBuffer.data(), 
                    textBuffer.size(), 
                    ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, 
                    EditorGraphListWindow::handleExpressionCursorPosCallback, 
                    &currentExpressionTextBuffer);                
            }

            if (textChanged) {
                controller->parseThenEvaluate(model, math::GraphLimits::GlobalLimits);
            }

            ImGui::PopFont();

            // Revert color changes
            const auto popColor = static_cast<std::int32_t>(currentExpressionIsSelected || !currentExpression.isValid());
            ImGui::PopStyleColor(popColor);

            // Set current expression by clicking on textbox 
            if (ImGui::IsItemActive() && ImGui::IsItemClicked()) {
                controller->setSelected(model);
            }
        }


        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::EndGroup();

        if (ImGui::IsItemClicked() && !ImGui::IsAnyItemActive()) {
            controller->setSelected(model);
        }

        ImGui::Spacing();
    }

    void EditorGraphListWindow::drawGraphListHeader() {
        const auto region = ImGui::GetContentRegionAvail();
        const auto buttonSize = ImVec2(0, region.y);
        if (ImGui::Button("Add", buttonSize)) {
            controller->create();
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("A button which you can add new graph.");
        }

        ImGui::SameLine(region.x - 69.0f);
        ImGui::SetNextItemWidth(region.x - 55.0f);

        if (ImGui::Button("Clear All", buttonSize)) {
            controller->clear();
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Clear all your graphs.");
        }
        // TODO: Undo redo buttons
    }

    void EditorGraphListWindow::onRender(kubvc::render::GUI& gui) {
        const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar /* |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding */;

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