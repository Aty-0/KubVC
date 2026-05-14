#include "editor_graph_edit_window.h"
#include "vec_convert.h"
#include "expression_controller.h"
#include "logger.h"
#include <fstream>

//#define SHOW_DEBUG_TOOLS_ON_RELEASE
inline static ImVec4 operator*(const ImVec4& l, std::int32_t r) {
    return { l.x * r, l.y * r, l.z * r, l.w * r, }; 
}

namespace kubvc::editor { 
    static const auto controller = math::ExpressionController::getInstance();

    EditorEditGraphWindow::EditorEditGraphWindow() {
        setName("Edit Graph##EditorEditGraphWindow");
    }

#if defined(KUB_IS_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE) 
    static void showTreeList(const std::shared_ptr<kubvc::algorithm::INode> start) {
        static constexpr auto TREE_NODE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen;

        // We are reached the end of tree 
        if (start == nullptr) {
            return;
        }

        const auto type = start->getType();
        const auto nodeName = algorithm::getNodeName(start->getType());
        switch (type) {
            case algorithm::NodeTypes::Root: {
                const auto node = castToNodePtr<algorithm::NodeTypes::Root>(start);
                if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                    if (!node->child) {
                        ImGui::Text("Child is empty");
                    } else {
                        showTreeList(node->child);
                    }
                    ImGui::TreePop();  
                }  
                break;
            }
            case algorithm::NodeTypes::Operator: {
                const auto node = castToNodePtr<algorithm::NodeTypes::Operator>(start);  
                if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                    ImGui::Text("Left");
                    // Recursively clear left and right subtrees
                    if (node->left != nullptr) {
                        showTreeList(node->left);
                    }

                    ImGui::Text("Right");
                    if (node->right != nullptr) {
                        showTreeList(node->right);
                    }
                    ImGui::TreePop();  
                }  


                break;    
            }
            case algorithm::NodeTypes::UnaryOperator: {
                const auto node = castToNodePtr<algorithm::NodeTypes::UnaryOperator>(start);   

                if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                    if (node->child != nullptr) {
                        showTreeList(node->child);
                    }
                    ImGui::TreePop();  
                } 

                break;     
            }
            case algorithm::NodeTypes::Function: {
                const auto node = castToNodePtr<algorithm::NodeTypes::Function>(start);    
                
                ImGui::Text("%s", node->name.c_str());
                if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                    if (node->argument != nullptr) {
                        showTreeList(node->argument);
                    }
                    ImGui::TreePop();  
                } 

                break;
            }
            case algorithm::NodeTypes::Number: {
                const auto node = castToNodePtr<algorithm::NodeTypes::Number>(start);    
                ImGui::Text("%f", node->getValue());
                break;
            }
            case algorithm::NodeTypes::Variable: {
                const auto node = castToNodePtr<algorithm::NodeTypes::Variable>(start);    
                ImGui::Text("%c", node->getValue());
                ImGui::SameLine();
                ImGui::Text("isParameter: %i", node->isParameter);
                ImGui::Text("value: %f", node->parameter);
                break;
            }
            case algorithm::NodeTypes::Invalid: {
                ImGui::Text("invalid, id:%d", start->getId());
                break;
            }
            default:
                KUB_ASSERT(true, "Unknown type");
                break;
        }

    }

    static void drawDebugAST() {
        if (ImGui::CollapsingHeader("AST")) {
            const auto selected = controller->getSelected();
            if (selected != nullptr) {
                ImGui::Text("Current tree is %s", selected->getTextBuffer().getBuffer().data());   

                auto& expression = selected->getExpression();
                auto& tree = expression.getTree();
                showTreeList(tree.getRoot());
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
        const auto selected = controller->getSelected();
        if (selected != nullptr && selected->getSettings().getChangeColor()) {
            auto& settings = selected->getSettings();
            auto color = settings.getColor();
            if (ImGui::ColorPicker4("##_CurrentExprColorPicker", &color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel)) {
                settings.setColor(color);
            }
        }
    }
    
    static inline void drawIcon(const kubvc::render::GUI& gui, std::string_view icon) {
        ImGui::PushFont(&gui.getIconFont());
        ImGui::Text(icon.data());
        ImGui::PopFont();
    }

    void EditorEditGraphWindow::onRender(kubvc::render::GUI& gui) {
        ImGui::TextDisabled("Current graph settings");
        ImGui::Separator();
        
        const auto selected = controller->getSelected();
        if (selected != nullptr) {
            ImGui::Dummy(ImVec2(0, 5.0f));
            drawIcon(gui, ICON_FA_WAVE_SQUARE);
            ImGui::SameLine(0, 10.0f);
            ImGui::TextDisabled("Graph: %s", selected->getTextBuffer().getBuffer().data());
            ImGui::Dummy(ImVec2(0, 15.0f));

            // Style settings block
            ImGui::TextDisabled("Style");
            ImGui::Dummy(ImVec2(0, 5.0f));
            
            // Visible toggle 
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_EYE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
            

            auto& settings = selected->getSettings();
            auto visible = settings.getVisible();
            if (ImGui::Checkbox("Visible##OptionsGraphVisibleCheckBox", &visible)) {
                settings.setVisible(visible);
            }
            
            // Shaded toggle
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_CIRCLE_HALF_STROKE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);

            auto shaded = settings.getShaded();
            if (ImGui::Checkbox("Shaded##OptionsGraphShadedCheckBox", &shaded)) {
                settings.setShaded(shaded);   
            }
            
            // Color picker
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_PALETTE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
            auto color = kubvc::utility::toImVec4(settings.getColor());
            if (ImGui::ColorButton("##OptionsGraphColorPicker", color, ImGuiColorEditFlags_NoBorder, ImVec2(25, 25))) {
                settings.setChangeColor(!settings.getChangeColor());
            }
            ImGui::SameLine();
            ImGui::Text("Color");
            
            drawLineColorPicker();

            // Line thickness
            ImGui::Dummy(ImVec2(0, 10.0f));
            ImGui::TextDisabled("Thickness");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_BRUSH);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70.0f);
            auto thickness = settings.getThickness();
            if (ImGui::SliderFloat("##OptionsGraphThicknessDrag", &thickness, THICKNESS_MIN, 
                THICKNESS_MAX, "%.1f", ImGuiSliderFlags_::ImGuiSliderFlags_ClampOnInput)) {
                settings.setThickness(thickness);
            }

#if defined(KUB_IS_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE)  
            ImGui::Separator();        
            drawIcon(gui, ICON_FA_BUG);
            ImGui::SameLine();        
            ImGui::TextDisabled("Debug");
            drawDebugAST();
            const auto points = selected->getExpression().getPlotBuffer();
            ImGui::Text("Count: %zu", points.size());
            if (ImGui::CollapsingHeader("Points")) {
                for (auto point : selected->getExpression().getPlotBuffer()) {
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