#include "editor_graph_list_window.h"
#include "expression_controller.h"
#include "application_config.h"
#include "primitives.h"

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
    
    void EditorGraphListWindow::drawParameterList(std::shared_ptr<math::ExpressionModel> model) {
        if (!model) {
            return;
        }
        auto& expression = model->getExpression();
        auto& vdc = expression.getVDC();
        const auto& parameters = vdc.getParameterVariables();
        if (!parameters.empty() && expression.isValid()) {
            ImGui::Separator();
            for (auto node : parameters) {
                if (node && node->isParameter) {
                    const auto value = node->getValue();
                    static bool useTime = false; // FIXME:
                    ImGui::Text("Parameter: %c", value);
                    
                    const auto dragFloatName = std::format("Value##ValueDragParam{}_{}", std::string(1, value), node->getId());
                    if (useTime) {
                        ImGui::BeginDisabled();
                        ImGui::DragFloat(dragFloatName.data(), &node->parameter);
                        node->parameter += ImGui::GetIO().DeltaTime;                        
                        ImGui::EndDisabled();
                        expression.eval(math::GraphLimits::GlobalLimits);
                    } else {
                        if (ImGui::DragFloat(dragFloatName.data(), &node->parameter)) {
                            expression.eval(math::GraphLimits::GlobalLimits);
                        }
                    }

                    ImGui::SameLine();
                    const auto checkBoxName = std::format("Use time##UseTimeForParam{}_{}", std::string(1, value), node->getId());
                    ImGui::Checkbox(checkBoxName.data(), &useTime);

                } else {
                    ImGui::Text("Invalid parameter");
                }
                ImGui::Separator();
            }
        }
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
                drawParameterList(model);
            }
        }
    }

    void EditorGraphListWindow::drawGraphPanel(kubvc::render::GUI& gui, std::shared_ptr<math::ExpressionModel> model, std::int32_t index) {
        static auto& fontBig = gui.getDefaultFontMathSize();        
        if (!model) {
            return;
        }
        
        auto& currentExpression = model->getExpression();
        auto& currentSettings = model->getSettings();
        
        const auto selectedModel = controller->getSelected();
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
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)); 
        ImGui::BeginChild(("##graphPanel" + idStr).c_str(), ImVec2(0, totalHeight), ImGuiChildFlags_::ImGuiChildFlags_Borders);
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_Border));
        ImGui::BeginChild(("##gripArea" + idStr).c_str(), ImVec2(24 * scale, 0));
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.20f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.28f, 0.31f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.38f, 0.95f));
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (totalHeight - ImGui::GetFrameHeight()) / 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(4);
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginGroup();
        
        // Draw current index text 
        {
            ImGui::PushFont(&fontBig);
            ImGui::TextDisabled("%d", index);
            ImGui::PopFont();
            ImGui::SameLine();
        }

        ImGui::SameLine();

        // Draw icons
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
                // Draw error hint 
                {
                    const auto lastErrorMessage = currentExpression.getLastErrorMessage();
                    if (!currentExpression.isValid() && !lastErrorMessage.empty()) {
                        ImGui::SameLine();
                        ImGui::Dummy(ImVec2(2.0f, 0.0f)); 
                        ImGui::SameLine();

                        ImGui::AlignTextToFramePadding();

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

        // Draw textbox 
        {
            ImGui::PushFont(&gui.getMathFont());
            // FIXME: for some reason not work
            if (!currentExpression.isValid())
                ImGui::PushStyleColor(ImGuiCol_Border, INVALID_COLOR);
            else if (currentExpressionIsSelected)
                ImGui::PushStyleColor(ImGuiCol_Border, SELECTED_COLOR);
                    
            
            const auto textBoxWidth = ImGui::GetContentRegionAvail().x - 8 * scale;
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
        
        ImGui::PopStyleColor();
        ImGui::EndGroup();
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::EndGroup();

        if (ImGui::IsItemClicked() && !ImGui::IsAnyItemActive()) {
            controller->setSelected(model);
        }
        
        static const auto appConfig = application::ApplicationConfig::getInstance();
        
        if (appConfig->getMode() == application::MathMode::Complex) {
            constexpr auto DRAG_SPEED = 0.01f; 
            auto isRectMode = currentExpression.getRectMode();
            if (ImGui::Checkbox(("Grid Mode" + ("##GridModeCheckBox" + idStr)).c_str(), &isRectMode)) {
                currentExpression.setRectMode(isRectMode);
                currentExpression.eval(math::GraphLimits::GlobalLimits);
            }

            if (!isRectMode) {            
                const auto primitiveType = currentExpression.getPrimitiveType();
                switch (currentExpression.getPrimitiveType()) {
                    case math::primitives::PrimitiveTypes::Circle: {
                        const auto primitive = currentExpression.getPrimitive<math::primitives::CirclePrimitive>();
                        if (ImGui::DragScalar(("Radius" + ("##CircleRadiusDrag" + idStr)).c_str(), ImGuiDataType_Double, &primitive->radius, DRAG_SPEED)) {
                            primitive->generate(math::Expression::MAX_PLOT_BUFFER_SIZE);
                            currentExpression.eval(math::GraphLimits::GlobalLimits);
                        }

                        if (ImGui::DragScalarN(("Center" + ("##CircleCenterDrag" + idStr)).c_str(),  ImGuiDataType_Double, &primitive->center, 2, DRAG_SPEED)) {
                            primitive->generate(math::Expression::MAX_PLOT_BUFFER_SIZE);
                            currentExpression.eval(math::GraphLimits::GlobalLimits);
                        }
                        break;
                    }
                    case math::primitives::PrimitiveTypes::Rectangle: {
                        const auto primitive = currentExpression.getPrimitive<math::primitives::RectanglePrimitive>();
                        auto rect = primitive->rect;
                        if (ImGui::DragScalarN(("Rect" + ("##RectangleRectDrag" + idStr)).c_str(), ImGuiDataType_Double, &rect, 4, DRAG_SPEED)) {
                            primitive->rect = rect;
                            primitive->generate(math::Expression::MAX_PLOT_BUFFER_SIZE);
                            currentExpression.eval(math::GraphLimits::GlobalLimits);
                        }
                        break;            
                    }
                }
                

                // TODO: Not a great impl and kinda dumb, but it's fine for nown
                static const std::vector<std::string> options = { "Circle", "Rectangle" };
                auto current = static_cast<std::int32_t>(primitiveType);
                if (ImGui::BeginCombo(("Primitive Type" + ("##PrimitiveTypeCombo" + idStr)).c_str(), options[current].c_str())) {
                    for (std::int32_t i = 0; i < static_cast<std::int32_t>(options.size()); i++) {
                        if (ImGui::Selectable(options[i].c_str(), current == i)) {
                            currentExpression.setPrimitiveType(static_cast<math::primitives::PrimitiveTypes>(i));

                            switch (currentExpression.getPrimitiveType()) {
                                case math::primitives::PrimitiveTypes::Circle: {
                                    currentExpression.setNewPrimitive(math::primitives::makeNewPrimitive<math::primitives::CirclePrimitive>(math::Expression::MAX_PLOT_BUFFER_SIZE));
                                    break;
                                }
                                case math::primitives::PrimitiveTypes::Rectangle: {
                                    currentExpression.setNewPrimitive(math::primitives::makeNewPrimitive<math::primitives::RectanglePrimitive>(math::Expression::MAX_PLOT_BUFFER_SIZE));
                                    break;            
                                }
                            }

                            currentExpression.eval(math::GraphLimits::GlobalLimits);
                        }
                    }
                    ImGui::EndCombo();
                }
            } else {
                // FIXME: Bruh
                std::array<float, 4> limits = { 
                    static_cast<float>(math::GraphLimits::GlobalLimits.xMin), 
                    static_cast<float>(math::GraphLimits::GlobalLimits.xMax), 
                    static_cast<float>(math::GraphLimits::GlobalLimits.yMin), 
                    static_cast<float>(math::GraphLimits::GlobalLimits.yMax) 
                };
                
                if (ImGui::InputFloat4(("Rectangle surface" + ("##RectSurfaceInput" + idStr)).c_str(), limits.data())) {
                    math::GraphLimits::GlobalLimits = math::GraphLimits { limits[0], limits[1], limits[2], limits[3] };
                    currentExpression.eval(math::GraphLimits::GlobalLimits);
                }
            }
            
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
        constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        constexpr auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar /* |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding */;

        auto windowSize = ImGui::GetWindowSize();
        if (ImGui::BeginChild("GraphListHeader", ImVec2(windowSize.x, 42.0f), childFlags)) {
            drawGraphListHeader();
        }
        ImGui::EndChild();
        
        if (ImGui::BeginChild("GraphListChild", ImVec2(windowSize.x, 0), childFlags, childWindowFlags)) { 
            drawGraphList(gui);
        }
        ImGui::EndChild();
    }
}