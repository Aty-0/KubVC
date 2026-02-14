#include "editor_graph_edit_window.h"
#include "vec_convert.h"
#include "expression.h"
#include "logger.h"
#include <fstream>

#define SHOW_DEBUG_TOOLS_ON_RELEASE
extern ImVec4 operator*(const ImVec4& l, std::int32_t r) {
    return { l.x * r, l.y * r, l.z * r, l.w * r, }; 
}

namespace kubvc::editor {    
    EditorEditGraphWindow::EditorEditGraphWindow() {
        setName("Edit Graph");
    }

#if defined(KUB_IS_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE) 
    static void showTreeList(std::shared_ptr<kubvc::algorithm::INode> start) {
        // We are reached the end of tree 
        if (start == nullptr) {
            return;
        }

        auto type = start->getType();
        auto nodeName = std::string();

        static const auto flags = ImGuiTreeNodeFlags_DefaultOpen;
        switch (type)
        {
            case kubvc::algorithm::NodeTypes::Root: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Root>(start);
                nodeName = "Root id:" + std::to_string(node->getId());
                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    showTreeList(node->child);
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Number: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Number>(start);         
                nodeName = std::to_string(node->getValue()) + " id:" + std::to_string(node->getId());

                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    ImGui::TreePop();  
                }  
                break;            
            }
            case kubvc::algorithm::NodeTypes::Operator: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Operator>(start);         
                nodeName = std::string(1, node->operation) + " id:" + std::to_string(node->getId()); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    ImGui::Text("Left");
                    showTreeList(node->left);            
                    ImGui::Text("Right");
                    showTreeList(node->right);
                    ImGui::TreePop();  
                }  
                break;    
            }
            case kubvc::algorithm::NodeTypes::Variable: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Variable>(start);         
                nodeName = std::string(1, node->getValue()) + " id:" + std::to_string(node->getId()); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::UnaryOperator>(start);         
                nodeName = std::string(1, node->operation) + " id:" + std::to_string(node->getId()); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    showTreeList(node->child);     
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Function: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Function>(start);         
                nodeName = node->name + " id:"  +std::to_string(node->getId()); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flags)) {
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Invalid: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Invalid>(start);         
                nodeName = node->name + " id:" + std::to_string(node->getId());   
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Invalid");
                break;
            }
            default:
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "UNK");
                break;
        }
    }

    static constexpr auto MOVE = 100.0f;

    static void showTree(const std::shared_ptr<kubvc::algorithm::INode>& start,
        ImDrawList* drawList, ImVec2 origin, ImVec2 panOffset,
        ImVec2 pos, bool hasParent, float spacing)
    {
        if (start == nullptr) {
            return;
        } 
        
        auto& style = ImGui::GetStyle();

        const auto lineColor = style.Colors[ImGuiCol_::ImGuiCol_PlotLines] * 100;        
        const auto lineColor32 = IM_COL32(lineColor.x, lineColor.y, lineColor.z, 255);
        const auto textColor = style.Colors[ImGuiCol_::ImGuiCol_Text] * 100;   
        const auto textColor32 = IM_COL32(textColor.x, textColor.y, textColor.z, 255);

        auto nodePos = ImVec2(origin.x + panOffset.x + pos.x, origin.y + panOffset.y + pos.y);
        std::string label;
        switch (start->getType()) {
            case kubvc::algorithm::NodeTypes::Root: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Root>(start);   
                label = "#" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());

                if (node->child) {
                    hasParent = true;
                    auto childPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(node->child, drawList, origin, panOffset, childPos, true, spacing);
                    auto childAbs = ImVec2(origin.x + panOffset.x + childPos.x, origin.y + panOffset.y + childPos.y);
                    drawList->AddLine(nodePos, childAbs, lineColor32, 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Number: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Number>(start);   
                label = std::to_string(node->getValue()) + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());
                break;
            }
            case kubvc::algorithm::NodeTypes::Variable: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Variable>(start);   
                label = std::string(1, node->getValue()) + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());
                break;
            }
            case kubvc::algorithm::NodeTypes::Operator: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Operator>(start);   
                label = std::string(1, node->operation) + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());

                if (node->left) {
                    auto leftPos = ImVec2(pos.x - spacing * 2, pos.y + spacing);
                    showTree(node->left, drawList, origin, panOffset, leftPos, true, spacing);
                    auto leftAbs = ImVec2(origin.x + panOffset.x + leftPos.x, origin.y + panOffset.y + leftPos.y);
                    drawList->AddLine(nodePos, leftAbs, lineColor32, 2.0f);
                }

                if (node->right) {
                    auto rightPos = ImVec2(pos.x + spacing * 2, pos.y + spacing);
                    showTree(node->right, drawList, origin, panOffset, rightPos, true, spacing);
                    auto rightAbs = ImVec2(origin.x + panOffset.x + rightPos.x, origin.y + panOffset.y + rightPos.y);
                    drawList->AddLine(nodePos, rightAbs, lineColor32, 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::UnaryOperator>(start);   
                label = std::string(1, node->operation) + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());

                if (node->child) {
                    auto childPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(node->child, drawList, origin, panOffset, childPos, true, spacing);
                    auto childAbs = ImVec2(origin.x + panOffset.x + childPos.x, origin.y + panOffset.y + childPos.y);
                    drawList->AddLine(nodePos, childAbs, lineColor32, 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Function: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Function>(start);   
                label = node->name + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, textColor32, label.c_str());

                if (node->argument) {
                    auto argPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(node->argument, drawList, origin, panOffset, argPos, true, spacing);
                    auto argAbs = ImVec2(origin.x + panOffset.x + argPos.x, origin.y + panOffset.y + argPos.y);
                    drawList->AddLine(nodePos, argAbs, lineColor32, 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Invalid: {
                const auto node = algorithm::castToNodePtr<algorithm::NodeTypes::Invalid>(start);   
                label = node->name + " #" + std::to_string(node->getId());
                drawList->AddText(nodePos, IM_COL32(0, 255, 0, 255), label.c_str());
                break;
            }
            default:
                drawList->AddText(nodePos, IM_COL32(255, 0, 0, 255), "UNK");
                break;
        }

        if (hasParent) {
            drawList->AddCircleFilled(nodePos, 3.0f, IM_COL32(255, 0, 0, 255));
        }
    }

    static void showTreeVisual(const kubvc::algorithm::ASTree& tree) {            
        static constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders; 
        static auto dragOffset = ImVec2(0,0);
        //static auto width = 0.65f;
        const auto origin = ImGui::GetCursorScreenPos(); // Top-left of the child window
        const auto drawList = ImGui::GetWindowDrawList();
        // TODO: Soo, how we can implement zoom
        //ImGui::SliderFloat("Width##TreeChildWindow", &width, 0.1f, 2.0f);

        if (ImGui::BeginChild("TreeChildWindow", ImVec2(0,0), childFlags, ImGuiWindowFlags_NoInputs)) { 
            auto min = ImGui::GetWindowPos();
            auto max = ImVec2(min.x + ImGui::GetWindowWidth(), min.y + ImGui::GetWindowHeight());
            drawList->PushClipRect(min, max, true);
            //ImGui::PushItemWidth(ImGui::GetWindowWidth() * width);
            ImGui::Text("drag %f %f", dragOffset.x, dragOffset.y);
            const auto root = const_cast<kubvc::algorithm::ASTree&>(tree).getRoot();
            showTree(algorithm::castToINodePtr<algorithm::NodeTypes::Root>(root), drawList, origin, dragOffset, ImVec2(0, 0), false, MOVE);
            //ImGui::PopItemWidth();
            drawList->PopClipRect();
        }   

        auto io = ImGui::GetIO();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Right)) {
            dragOffset.x += io.MouseDelta.x;
            dragOffset.y += io.MouseDelta.y;
        }
        ImGui::EndChild();
    }

    static void drawDebugAST() {
        if (ImGui::CollapsingHeader("AST")) {
            auto selected = kubvc::math::ExpressionController::Selected;
            if (selected != nullptr) {
                ImGui::Text("Current tree is %s", selected->getTextBuffer().data());   

                static bool listStyleTree = false;
                ImGui::Checkbox("List style for tree", &listStyleTree);
                if (listStyleTree) {
                    showTreeList(selected->getTree().getRoot());
                }
                else {
                    showTreeVisual(selected->getTree());
                }
            }
            else {
                ImGui::Text("No currently selected tree");
            }
        }
        else {
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Show a abstract syntax tree for current graph. Only for debug purposes");
            }
        }
    }
#endif

    void EditorEditGraphWindow::drawLineColorPicker() {
        auto selected = kubvc::math::ExpressionController::Selected;
        if (selected != nullptr && selected->Settings.changeColor) {
            ImGui::ColorPicker4("##_CurrentExprColorPicker", &selected->Settings.color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);
        }
    }
    
    static inline void drawIcon(const kubvc::render::GUI& gui, std::string_view icon) {
        ImGui::PushFont(gui.getIconFont());
        ImGui::Text(icon.data());
        ImGui::PopFont();
    }

    void EditorEditGraphWindow::onRender(kubvc::render::GUI& gui) {
        ImGui::TextDisabled("Current graph settings");
        ImGui::Separator();
        
        auto selected = kubvc::math::ExpressionController::Selected;
        if (selected != nullptr) {
            ImGui::Dummy(ImVec2(0, 5.0f));
            drawIcon(gui, ICON_FA_WAVE_SQUARE);
            ImGui::SameLine(0, 10.0f);
            ImGui::TextDisabled("Graph: %s", selected->getTextBuffer().data());
            ImGui::Dummy(ImVec2(0, 15.0f));

            // Style settings block
            ImGui::TextDisabled("Style");
            ImGui::Dummy(ImVec2(0, 5.0f));
            
            // Visible toggle 
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_EYE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
            bool visible = selected->isVisible();
            if (ImGui::Checkbox("Visible##OptionsGraphVisibleCheckBox", &visible)) {
                selected->setVisible(visible);
            }
            
            // Shaded toggle
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_CIRCLE_HALF_STROKE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
            ImGui::Checkbox("Shaded##OptionsGraphShadedCheckBox", &selected->Settings.shaded);
            
            // Color picker
            if (selected->Settings.isRandomColorSetted) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                drawIcon(gui, ICON_FA_PALETTE);
                ImGui::SameLine(0, 10.0f);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
                auto color = kubvc::utility::toImVec4(selected->Settings.color);
                if (ImGui::ColorButton("##OptionsGraphColorPicker", color, ImGuiColorEditFlags_NoBorder, ImVec2(25, 25))) {
                    selected->Settings.color = kubvc::utility::toGlmVec4(color);
                    selected->Settings.changeColor = !selected->Settings.changeColor;
                }
                ImGui::SameLine();
                ImGui::Text("Color");
            }
            drawLineColorPicker();

            // Line thickness
            ImGui::Dummy(ImVec2(0, 10.0f));
            ImGui::TextDisabled("Thickness");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_BRUSH);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70.0f);
            if (ImGui::SliderFloat("##OptionsGraphThicknessDrag", &selected->Settings.thickness, THICKNESS_MIN, THICKNESS_MAX, "%.1f")) {
                if (selected->Settings.thickness > THICKNESS_MAX) {
                    selected->Settings.thickness = THICKNESS_MAX;
                } 
                else if (selected->Settings.thickness < THICKNESS_MIN) {
                    selected->Settings.thickness = THICKNESS_MIN;
                }     
            }

#if defined(KUB_IS_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE)  
            ImGui::Separator();        
            drawIcon(gui, ICON_FA_BUG);
            ImGui::SameLine();        
            ImGui::TextDisabled("Debug");
            drawDebugAST();
            const auto points = selected->getPlotBuffer();
            ImGui::Text("Count: %i", points.size());
            if (ImGui::CollapsingHeader("Points")) {
                if (ImGui::Button("Dump points to file")) {
                    std::ofstream filex;
                    std::ofstream filey;
                    std::ofstream filepoints;
                    filex.open("dump_x.txt", std::ios_base::out);
                    filey.open("dump_y.txt", std::ios_base::out);
                    filepoints.open("filepoints.txt", std::ios_base::out);
                    
                    for (auto point : points) {
                        filex << point.x << "\n";
                        filey << point.y << "\n";
                        filepoints << "(" << point.x << "," << point.y  << ")" << ",";
                        //KUB_DEBUG("[Dump] x:%f y:%f", point.x, point.y);
                    }

                    filex.close();
                    filey.close();
                    filepoints.close();
                }
                ImGui::Separator();
                for (auto point : selected->getPlotBuffer()) {
                    if (glm::isnan(point.x) || glm::isnan(point.y)) {
                        ImGui::TextColored(ImVec4(1,0,0,1), "x:%f y:%f", point.x, point.y);
                    }
                    else {
                        ImGui::TextDisabled("x:%f y:%f", point.x, point.y);
                    }
                }
            }
#endif
        }
        else {
            ImGui::TextDisabled("None");
        }
    }       
}