#include "editor_graph_edit_window.h"
#include "vec_convert.h"
#include "expression.h"
#include "logger.h"

namespace kubvc::editor
{    
    EditorEditGraphWindow::EditorEditGraphWindow()
    {
        setName("Edit Graph");
    }
    
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

    static const auto MOVE = 100.0f;

    static void showTree(std::shared_ptr<kubvc::algorithm::Node> start, ImVec2 offset, ImVec2 pos = ImVec2(0,0), ImVec2 prev_pos = ImVec2(0,0))
    {
        // We are reached the end of tree 
        if (start == nullptr)
        {
            return;
        }

        auto type = start->getType();
        auto nodeName = std::string();

        switch (type)
        {
            case kubvc::algorithm::NodeTypes::Root:
            {
                auto node = static_cast<kubvc::algorithm::RootNode*>(start.get());
                nodeName = ". #" + std::to_string(node->id);

                ImGui::SetCursorPos(ImVec2(offset.x, offset.y));
                ImGui::Text(nodeName.c_str());
                showTree(node->child, ImVec2(offset.x, offset.y), ImVec2(0, MOVE));
                break;
            }
            case kubvc::algorithm::NodeTypes::Number:
            {
                auto node = static_cast<kubvc::algorithm::NumberNode*>(start.get());         
                nodeName = std::to_string(node->value) + " #" + std::to_string(node->id);
                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::Text(nodeName.c_str());
                break;            
            }
            case kubvc::algorithm::NodeTypes::Operator:
            {
                auto node = static_cast<kubvc::algorithm::OperatorNode*>(start.get());         
                nodeName = std::string(1, node->operation) +  " #" + std::to_string(node->id);

                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::Text(nodeName.c_str());
                if (node->left != nullptr)
                {
                    showTree(node->left, offset, node->left->getType() == kubvc::algorithm::NodeTypes::Operator 
                        ? ImVec2(pos.x + -MOVE * 2, pos.y + MOVE) : ImVec2(pos.x + -MOVE, pos.y + MOVE), pos);                                    
                }

                if (node->right != nullptr)
                {
                    showTree(node->right, offset, node->right->getType() == kubvc::algorithm::NodeTypes::Operator 
                        ? ImVec2(pos.x + MOVE * 2, pos.y + MOVE) : ImVec2(pos.x + MOVE, pos.y + MOVE), pos);                                 
                }

                break;    
            }
            case kubvc::algorithm::NodeTypes::Variable:
            {
                auto node = static_cast<kubvc::algorithm::VariableNode*>(start.get());         
                nodeName = node->value + " #" + std::to_string(node->id); 
                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::Text(nodeName.c_str());
                break;
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator:
            {
                auto node = static_cast<kubvc::algorithm::UnaryOperatorNode*>(start.get());         
                nodeName = std::string(1, node->operation) + " id:" + std::to_string(node->id); 
                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::Text(nodeName.c_str());

                showTree(node->child, offset, ImVec2(pos.x, pos.y + MOVE), pos);     
                break;
            }
            case kubvc::algorithm::NodeTypes::Function:
            {
                auto node = static_cast<kubvc::algorithm::FunctionNode*>(start.get());         
                nodeName = node->name + " #" + std::to_string(node->id); 
                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::Text(nodeName.c_str());
                ImGui::SameLine();
                if (node->argument != nullptr)
                {
                    ImGui::Text("Args tree:");
                    showTree(node->argument, offset, ImVec2(pos.x, pos.y + MOVE), pos);     
                }
                else 
                {
                    ImGui::Text("(INV_ARG)");
                }


                break;
            }
            case kubvc::algorithm::NodeTypes::Invalid:
            {
                auto node = static_cast<kubvc::algorithm::InvalidNode*>(start.get());         
                nodeName = node->name + " #" + std::to_string(node->id); 
                const auto size = nodeName.size();
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x - size, offset.y + pos.y));
                ImGui::TextColored(ImVec4(255, 0, 0, 255), nodeName.c_str());
                break;
            }
            default:
                ImGui::SetCursorPos(ImVec2(offset.x + pos.x, offset.y + pos.y));
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "UNK");
                //ERROR("Unknown type or not implemented");
                break;
        }

        auto regAvail = ImGui::GetWindowWidth() / ImGui::GetWindowHeight(); 
        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(ImVec2((offset.x + prev_pos.x) - regAvail, (offset.y + prev_pos.y + MOVE * 2) - regAvail), 
            ImVec2((offset.x + pos.x) - regAvail, (offset.y + pos.y + MOVE * 2) - regAvail), IM_COL32(255,255,255,150), 2.0f);

    }

    static void showTreeVisual(const kubvc::algorithm::ASTree& tree)
    {            
        static constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders; 
        static auto pos = ImVec2(0,0);
        static auto width = 0.65f;

        // TODO: Soo, how we can implement zoom
        //ImGui::SliderFloat("Width##TreeChildWindow", &width, 0.1f, 2.0f);

        if (ImGui::BeginChild("TreeChildWindow", ImVec2(0,0), childFlags, ImGuiWindowFlags_NoInputs))
        { 
            //ImGui::PushItemWidth(ImGui::GetWindowWidth() * width);
            showTree(tree.getRoot(), pos);
            //ImGui::PopItemWidth();
        }   

        auto io = ImGui::GetIO();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Right))
        {
            pos.x += io.MouseDelta.x;
            pos.y += io.MouseDelta.y;
        }
        ImGui::EndChild();
    }

    static void drawDebugAST()
    {
        if (ImGui::CollapsingHeader("AST debug"))
        {
            auto selected = kubvc::math::ExpressionController::Selected;
            if (selected != nullptr)
            {
                ImGui::Text("AST:");   
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
            
            ImGui::Separator();
            drawDebugAST();
            if (ImGui::CollapsingHeader("Debug points"))
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
        }
        else 
        {
            ImGui::TextDisabled("None");
        }
    }       
}