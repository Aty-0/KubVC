#include "editor_graph_edit_window.h"
#include "vec_convert.h"
#include "expression.h"
#include "logger.h"

//#define SHOW_DEBUG_TOOLS_ON_RELEASE

namespace kubvc::editor
{    
    EditorEditGraphWindow::EditorEditGraphWindow()
    {
        setName("Edit Graph");
    }

#if defined(_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE) 
    static void showTreeList(std::shared_ptr<kubvc::algorithm::Node> start)
    {
        // We are reached the end of tree 
        if (start == nullptr)
            return;

        auto type = start->getType();
        static const auto flag = ImGuiTreeNodeFlags_DefaultOpen;
        auto nodeName = std::string();

        switch (type)
        {
            case kubvc::algorithm::NodeTypes::Root:
            {
                auto node = static_cast<kubvc::algorithm::RootNode*>(start.get());
                nodeName = "Root id:" + std::to_string(node->id);
                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    showTreeList(node->child);
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Number:
            {
                auto node = static_cast<kubvc::algorithm::NumberNode*>(start.get());         
                nodeName = std::to_string(node->value) + " id:" + std::to_string(node->id);

                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    ImGui::TreePop();  
                }  
                break;            
            }
            case kubvc::algorithm::NodeTypes::Operator:
            {
                auto node = static_cast<kubvc::algorithm::OperatorNode*>(start.get());         
                nodeName = std::string(1, node->operation) + " id:" + std::to_string(node->id); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    ImGui::Text("Left");
                    showTreeList(node->left);            
                    ImGui::Text("Right");
                    showTreeList(node->right);
                    ImGui::TreePop();  
                }  
                break;    
            }
            case kubvc::algorithm::NodeTypes::Variable:
            {
                auto node = static_cast<kubvc::algorithm::VariableNode*>(start.get());         
                nodeName = node->value + " id:" +std::to_string(node->id); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator:
            {
                auto node = static_cast<kubvc::algorithm::UnaryOperatorNode*>(start.get());         
                nodeName = std::string(1, node->operation) + " id:" + std::to_string(node->id); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    showTreeList(node->child);     
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Function:
            {
                auto node = static_cast<kubvc::algorithm::FunctionNode*>(start.get());         
                nodeName = node->name + " id:" +std::to_string(node->id); 
                if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
                {
                    ImGui::TreePop();  
                }  
                break;
            }
            case kubvc::algorithm::NodeTypes::Invalid:
            {
                auto node = static_cast<kubvc::algorithm::InvalidNode*>(start.get());         
                nodeName = node->name + " id:" + std::to_string(node->id);   
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Invalid");
                break;
            }
            default:
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "UNK");
                break;
        }
    }

    static constexpr auto MOVE = 100.0f;

    static void showTree( const std::shared_ptr<kubvc::algorithm::Node>& node,
        ImDrawList* drawList, ImVec2 origin, ImVec2 panOffset,
        ImVec2 pos, bool hasParent, float spacing)
    {
        if (!node) 
            return;

        auto nodePos = ImVec2(origin.x + panOffset.x + pos.x, origin.y + panOffset.y + pos.y);
        std::string label;
        switch (node->getType())
        {
            case kubvc::algorithm::NodeTypes::Root:
            {
                auto* n = static_cast<kubvc::algorithm::RootNode*>(node.get());
                label = "#" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());

                if (n->child)
                {
                    hasParent = true;
                    auto childPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(n->child, drawList, origin, panOffset, childPos, true, spacing);
                    auto childAbs = ImVec2(origin.x + panOffset.x + childPos.x, origin.y + panOffset.y + childPos.y);
                    drawList->AddLine(nodePos, childAbs, IM_COL32(255, 255, 255, 150), 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Number:
            {
                auto* n = static_cast<kubvc::algorithm::NumberNode*>(node.get());
                label = std::to_string(n->value) + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());
                break;
            }
            case kubvc::algorithm::NodeTypes::Variable:
            {
                auto* n = static_cast<kubvc::algorithm::VariableNode*>(node.get());
                label = n->value + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());
                break;
            }
            case kubvc::algorithm::NodeTypes::Operator:
            {
                auto* n = static_cast<kubvc::algorithm::OperatorNode*>(node.get());
                label = std::string(1, n->operation) + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());

                if (n->left)
                {
                    auto leftPos = ImVec2(pos.x - spacing * 2, pos.y + spacing);
                    showTree(n->left, drawList, origin, panOffset, leftPos, true, spacing);
                    auto leftAbs = ImVec2(origin.x + panOffset.x + leftPos.x, origin.y + panOffset.y + leftPos.y);
                    drawList->AddLine(nodePos, leftAbs, IM_COL32(255, 255, 255, 150), 2.0f);
                }

                if (n->right)
                {
                    auto rightPos = ImVec2(pos.x + spacing * 2, pos.y + spacing);
                    showTree(n->right, drawList, origin, panOffset, rightPos, true, spacing);
                    auto rightAbs = ImVec2(origin.x + panOffset.x + rightPos.x, origin.y + panOffset.y + rightPos.y);
                    drawList->AddLine(nodePos, rightAbs, IM_COL32(255, 255, 255, 150), 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator:
            {
                auto* n = static_cast<kubvc::algorithm::UnaryOperatorNode*>(node.get());
                label = std::string(1, n->operation) + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());

                if (n->child)
                {
                    auto childPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(n->child, drawList, origin, panOffset, childPos, true, spacing);
                    auto childAbs = ImVec2(origin.x + panOffset.x + childPos.x, origin.y + panOffset.y + childPos.y);
                    drawList->AddLine(nodePos, childAbs, IM_COL32(255, 255, 255, 150), 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Function:
            {
                auto* n = static_cast<kubvc::algorithm::FunctionNode*>(node.get());
                label = n->name + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32_WHITE, label.c_str());

                if (n->argument)
                {
                    auto argPos = ImVec2(pos.x, pos.y + spacing);
                    showTree(n->argument, drawList, origin, panOffset, argPos, true, spacing);
                    auto argAbs = ImVec2(origin.x + panOffset.x + argPos.x, origin.y + panOffset.y + argPos.y);
                    drawList->AddLine(nodePos, argAbs, IM_COL32(255, 255, 255, 150), 2.0f);
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Invalid:
            {
                auto* n = static_cast<kubvc::algorithm::InvalidNode*>(node.get());
                label = n->name + " #" + std::to_string(n->id);
                drawList->AddText(nodePos, IM_COL32(255, 0, 0, 255), label.c_str());
                break;
            }
            default:
                drawList->AddText(nodePos, IM_COL32(255, 0, 0, 255), "UNK");
                break;
        }

        if (hasParent)
        {
            drawList->AddCircleFilled(nodePos, 3.0f, IM_COL32(255, 0, 0, 255));
        }
    }

    static void showTreeVisual(const kubvc::algorithm::ASTree& tree)
    {            
        static constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders; 
        static auto dragOffset = ImVec2(0,0);
        //static auto width = 0.65f;
        auto origin = ImGui::GetCursorScreenPos(); // Top-left of the child window
        auto drawList = ImGui::GetWindowDrawList();
        // TODO: Soo, how we can implement zoom
        //ImGui::SliderFloat("Width##TreeChildWindow", &width, 0.1f, 2.0f);

        if (ImGui::BeginChild("TreeChildWindow", ImVec2(0,0), childFlags, ImGuiWindowFlags_NoInputs))
        { 
            auto min = ImGui::GetWindowPos();
            auto max = ImVec2(min.x + ImGui::GetWindowWidth(), min.y + ImGui::GetWindowHeight());
            drawList->PushClipRect(min, max, true);
            //ImGui::PushItemWidth(ImGui::GetWindowWidth() * width);
            ImGui::Text("drag %f %f", dragOffset.x, dragOffset.y);
            showTree(tree.getRoot(), drawList, origin, dragOffset, ImVec2(0, 0), false, MOVE);
            //ImGui::PopItemWidth();
            drawList->PopClipRect();
        }   

        auto io = ImGui::GetIO();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Right))
        {
            dragOffset.x += io.MouseDelta.x;
            dragOffset.y += io.MouseDelta.y;
        }
        ImGui::EndChild();
    }

    static void drawDebugAST()
    {
        if (ImGui::CollapsingHeader("AST"))
        {
            auto selected = kubvc::math::ExpressionController::Selected;
            if (selected != nullptr)
            {
                ImGui::Text("Current tree is %s", selected->getTextBuffer().data());   

                static bool listStyleTree = false;
                ImGui::Checkbox("List style for tree", &listStyleTree);
                if (listStyleTree)
                {
                    showTreeList(selected->getTree().getRoot());
                }
                else
                {
                    showTreeVisual(selected->getTree());
                }
            }
            else 
            {
                ImGui::Text("No currently selected tree");
            }
        }
        else 
        {
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Show a abstract syntax tree for current graph. Only for debug purposes");
            }
        }
    }
#endif

    void EditorEditGraphWindow::drawLineColorPicker()
    {
        auto selected = kubvc::math::ExpressionController::Selected;
        if (selected != nullptr && selected->Settings.changeColor)
        {
            ImGui::ColorPicker4("##_CurrentExprColorPicker", &selected->Settings.color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);
        }
    }

    void EditorEditGraphWindow::onRender(kubvc::render::GUI* gui) 
    {
        ImGui::TextDisabled("Current graph settings");
        ImGui::Separator();      
        auto selected = kubvc::math::ExpressionController::Selected;
        if (selected != nullptr)
        {          
            ImGui::TextDisabled("Graph: %s", selected->getTextBuffer().data());
            ImGui::Dummy(ImVec2(0, 15.0f));
            
            ImGui::Text("Visible");
            ImGui::SameLine();
            bool visible = selected->isVisible();
            if (ImGui::Checkbox("##OptionsGraphVisibleCheckBox", &visible))
            {
                selected->setVisible(visible);
            }
            
            ImGui::Text("Shaded");
            ImGui::SameLine();
            ImGui::Checkbox("##OptionsGraphShadedCheckBox", &selected->Settings.shaded);
            // Do not show color editor when we are not generate random color    
            if (selected->Settings.isRandomColorSetted)
            {
                ImGui::Text("Color");
                ImGui::SameLine();
                auto color = kubvc::utility::toImVec4(selected->Settings.color);
                if (ImGui::ColorButton("##OptionsGraphColorPicker", color))
                {
                    selected->Settings.color = kubvc::utility::toGlmVec4(color);
                    selected->Settings.changeColor = !selected->Settings.changeColor;
                }
            }
            ImGui::Text("Line Thickness");
            ImGui::SameLine();
            ImGui::PushItemWidth(45.0f);
            if (ImGui::DragFloat("##OptionsGraphThicknessDrag", &selected->Settings.thickness, THICKNESS_SPEED, THICKNESS_MIN, THICKNESS_MAX, "%.1f"))
            {
                // Handle manualy writed value
                if (selected->Settings.thickness > THICKNESS_MAX)
                {
                    selected->Settings.thickness = THICKNESS_MAX;
                } 
                else if (selected->Settings.thickness < THICKNESS_MIN)
                {
                    selected->Settings.thickness = THICKNESS_MIN;
                }     
            }
            ImGui::PopItemWidth();
            drawLineColorPicker();
#if defined(_DEBUG) || defined(SHOW_DEBUG_TOOLS_ON_RELEASE)  
            ImGui::Separator();        
            ImGui::TextDisabled("Debug");
            drawDebugAST();
            if (ImGui::CollapsingHeader("Points"))
            {
                if (ImGui::Button("Dump points to log"))
                {
                    for (auto point : selected->getPlotBuffer())
                    {
                        DEBUG("[Dump] x:%f y:%f", point.x, point.y);
                    }
                }
                ImGui::Separator();
                for (auto point : selected->getPlotBuffer())
                {
                    if (glm::isnan(point.x) || glm::isnan(point.y))
                    {
                        ImGui::TextColored(ImVec4(1,0,0,1), "x:%f y:%f", point.x, point.y);
                    }
                    else 
                    {
                        ImGui::TextDisabled("x:%f y:%f", point.x, point.y);
                    }
                }
            }
#endif
        }
        else 
        {
            ImGui::TextDisabled("None");
        }
    }       
}