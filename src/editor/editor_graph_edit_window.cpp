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
    static void showTreeList(const algorithm::ASTree& tree) {
        static constexpr auto TREE_NODE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen;

        // We are reached the end of tree 
        auto cached = tree.getTreeCached();
        if (cached.empty()) {
            ImGui::Text("Tree are empty");
            return;
        }

        for (const auto& node : cached) {
            const auto type = node->getType();
            const auto nodeName = std::format("{} {}", algorithm::getNodeName(type), node->getId());

            if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                switch (type) {
                    case algorithm::NodeTypes::Operator: {
                        const auto operatorNode = algorithm::castToNodePtr<algorithm::NodeTypes::Operator>(node);   
                        if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                            ImGui::Text("operation: %c", operatorNode->operation);
                            ImGui::TreePop();  
                        }  
                        break;    
                    }
                    case algorithm::NodeTypes::UnaryOperator: {
                        const auto unaryNode = algorithm::castToNodePtr<algorithm::NodeTypes::UnaryOperator>(node);   

                        if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                            ImGui::Text("operation: %c", unaryNode->operation);
                            ImGui::TreePop();  
                        } 

                        break;     
                    }
                    case algorithm::NodeTypes::Function: {
                        const auto functionNode = algorithm::castToNodePtr<algorithm::NodeTypes::Function>(node);    
                        
                        if (ImGui::TreeNodeEx(nodeName.data(), TREE_NODE_FLAGS)) {
                            ImGui::Text("%s", functionNode->name.c_str());
                            ImGui::TreePop();  
                        } 

                        break;
                    }
                    case algorithm::NodeTypes::Number: {
                        const auto numberNode = algorithm::castToNodePtr<algorithm::NodeTypes::Number>(node);    
                        ImGui::Text("%f", numberNode->getValue());
                        break;
                    }
                    case algorithm::NodeTypes::Variable: {
                        const auto variableNode = algorithm::castToNodePtr<algorithm::NodeTypes::Variable>(node);    
                        ImGui::Text("%c", variableNode->getValue());
                        ImGui::SameLine();
                        ImGui::Text("isParameter: %i", variableNode->isParameter);
                        ImGui::Text("value: %f", variableNode->parameter);
                        break;
                    }
                    case algorithm::NodeTypes::ComplexNumber: {
                        ImGui::Text("complex number, id:%d", node->getId());
                        break;
                    }
                    case algorithm::NodeTypes::Invalid: {
                        ImGui::Text("invalid, id:%d", node->getId());
                        break;
                    }
                    default:
                        KUB_ASSERT(true, "Unknown type");
                        break;
                }
                ImGui::TreePop();  
            }            
        }
    }

    static void drawDebugAST() {
        if (ImGui::CollapsingHeader("AST")) {
            const auto selected = controller->getSelected();
            if (selected != nullptr) {
                ImGui::Text("Current tree is %s", selected->getTextBuffer()->getBuffer().data());   

                const auto& expression = selected->getExpression();
                auto& tree = expression->getTree();
                showTreeList(tree);
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
        const auto& settings = selected->getSettings();
        const auto isColorPickerOpenned = settings->getChangeColor();
        if (selected && isColorPickerOpenned) {
            auto color = settings->getColor();
            if (ImGui::ColorPicker4("##_CurrentExprColorPicker", &color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel)) {
                settings->setColor(color);
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
            ImGui::PushFont(&gui.getDefaultFontMathSize());
            ImGui::TextDisabled("Graph: %s", selected->getTextBuffer()->getBuffer().data());
            ImGui::PopFont();
            ImGui::Dummy(ImVec2(0, 15.0f));

            // Style settings block
            ImGui::TextDisabled("Style");
            ImGui::Dummy(ImVec2(0, 5.0f));
            
            // Visible toggle 
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_EYE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
            

            const auto& settings = selected->getSettings();
            auto visible = settings->getVisible();
            if (ImGui::Checkbox("Visible##OptionsGraphVisibleCheckBox", &visible)) {
                settings->setVisible(visible);
            }
            
            // Shaded toggle
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_CIRCLE_HALF_STROKE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);

            auto shaded = settings->getShaded();
            if (ImGui::Checkbox("Shaded##OptionsGraphShadedCheckBox", &shaded)) {
                settings->setShaded(shaded);   
            }
            
            // Color picker
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
            drawIcon(gui, ICON_FA_PALETTE);
            ImGui::SameLine(0, 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
            const auto color = kubvc::utility::toImVec4(settings->getColor());
            if (ImGui::ColorButton("##OptionsGraphColorPicker", color, ImGuiColorEditFlags_NoBorder, ImVec2(25, 25))) {
                settings->setChangeColor(!settings->getChangeColor());
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
            auto thickness = settings->getThickness();
            if (ImGui::SliderFloat("##OptionsGraphThicknessDrag", &thickness, THICKNESS_MIN, 
                THICKNESS_MAX, "%.1f", ImGuiSliderFlags_::ImGuiSliderFlags_ClampOnInput)) {
                settings->setThickness(thickness);
            }

#if defined(KUB_IS_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE)  
            ImGui::Separator();        
            drawIcon(gui, ICON_FA_BUG);
            ImGui::SameLine();        
            ImGui::TextDisabled("Debug");
            drawDebugAST();
            const auto& plotBufferPtr = selected->getExpression()->getPlotBuffer();
            if (plotBufferPtr) {
                const auto points = *plotBufferPtr;
                ImGui::Text("Count: %zu", points.size());
                if (ImGui::CollapsingHeader("Points")) {
                    for (const auto& point : points) {
                        if (glm::isnan(point.x) || glm::isnan(point.y)) {
                            ImGui::TextColored(ImVec4(1,0,0,1), "x:%f y:%f", point.x, point.y);
                        }
                        else {
                            ImGui::TextDisabled("x:%f y:%f", point.x, point.y);
                        }
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