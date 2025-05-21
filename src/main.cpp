#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "gui.h"
#include "expression.h"
#include "expression_parser.h"
#include <iterator>
#include <cstring>

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

static const auto INVALID_COLOR = ImVec4(0.64f, 0.16f, 0.16f, 1.0f); 
static const auto SELECTED_COLOR = ImVec4(0.16f, 0.64f, 0.16f, 1.0f); 
static const auto THICKNESS_MIN = 0.5f;
static const auto THICKNESS_MAX = 10.0f; 
static const auto THICKNESS_SPEED = 0.1f; 

// Save current cursor position for expression
static int handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data)
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

static void drawEditGraph(kubvc::render::GUI* gui, std::shared_ptr<kubvc::math::Expression> expr, const std::int32_t& id, const std::int32_t& index)
{
    static const auto fontBig = gui->getDefaultFontMathSize();
    
    // Draw counter 
    ImGui::PushFont(fontBig);
    ImGui::TextDisabled("%d:", index);
    ImGui::PopFont();
    
    ImGui::SameLine();

    ImGui::PushFont(gui->getMathFont());
    // Set special color for textbox border when we are selected expression or get invalid node somewhere kekw
    if (!expr->isValid())
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, INVALID_COLOR);
    else if (expr == kubvc::math::expressions::Selected)
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, SELECTED_COLOR);

    const auto idStr = std::to_string(id);

    if (ImGui::InputText(("##" + idStr + "_ExprInputText").c_str(), expr->getTextBuffer().data(), expr->getTextBuffer().size(), 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, handleExpressionCursorPosCallback, &expr))
    {
        expr->parseAndEval();
    }

    ImGui::PopFont();
    
    // Revert color changes
    auto popColor = static_cast<std::int32_t>(expr == kubvc::math::expressions::Selected || !expr->isValid());
    ImGui::PopStyleColor(popColor);

    // Set current expression by clicking on textbox 
    if (ImGui::IsItemActive() && ImGui::IsItemClicked())
    {
        kubvc::math::expressions::Selected = expr;
    }

    ImGui::SameLine();

    ImGui::PushFont(fontBig);
    ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
    if (ImGui::Button("-"))
    {
        auto it = std::find_if(kubvc::math::expressions::Expressions.begin(), kubvc::math::expressions::Expressions.end(), [expr](auto it) { return it->getId() == expr->getId(); });
        if (it != kubvc::math::expressions::Expressions.end())
        {
            // Set as nullptr to avoid some weird behaviour
            if (kubvc::math::expressions::Selected == expr)
            {
                kubvc::math::expressions::Selected = nullptr;
            }

            kubvc::math::expressions::Expressions.erase(it);
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
    auto show = expr->isShowing();
    if (ImGui::RadioButton("V", show))
    {
        expr->setShow(!show);
    }

    ImGui::PopID();
    ImGui::PopFont();
    
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Change visibility for this graph.");
    }
}

static void drawMainGraphPanel()
{
    auto region = ImGui::GetContentRegionAvail();
    
    if (ImGui::Button("Add"))
    {
        auto expr = std::make_shared<kubvc::math::Expression>();
        kubvc::math::expressions::Expressions.push_back(expr);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("A button which you can add new graph.");
    }

    ImGui::SameLine(region.x - 55.0f);
    ImGui::SetNextItemWidth(region.x - 55.0f);
    
    if (ImGui::Button("Clear All"))
    {
        kubvc::math::expressions::Selected = nullptr;
        kubvc::math::expressions::Expressions.clear();
        kubvc::math::expressions::Expressions.shrink_to_fit();
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Clear all your graphs.");
    }

    
    // TODO: Undo redo buttons
                
}

static void drawGraphList(kubvc::render::GUI* gui)
{
    std::int32_t expressionIndex = 0;
    for (auto expr : kubvc::math::expressions::Expressions)
    {
        if (expr != nullptr)
        {
            expressionIndex++;
            drawEditGraph(gui, expr, expr->getId(), expressionIndex);
        }
    }
}

static void drawDebugAST()
{
    if (ImGui::CollapsingHeader("AST debug"))
    {
        auto selected = kubvc::math::expressions::Selected;
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

static void drawLineColorPicker()
{
    auto selected = kubvc::math::expressions::Selected;
    if (selected != nullptr && selected->Settings.changeColor)
    {
        ImGui::ColorPicker4("##_CurrentExprColorPicker", &selected->Settings.color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);
    }
}

static void updateExpressionByPlotLimits(std::shared_ptr<kubvc::math::Expression> expr)
{
    auto limits = ImPlot::GetPlotLimits();                 
    expr->eval(limits.X.Max, limits.X.Min, limits.Y.Max, limits.Y.Min);
}

static void drawPlotter()
{
    auto size = ImGui::GetContentRegionAvail();
    const auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;
    if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) 
    {	
        // Draw axis notes 
        ImPlot::SetupAxis(ImAxis_X1, "X-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
        ImPlot::PushStyleColor(ImPlotCol_::ImPlotCol_AxisBgActive, ImVec4(255,0,0,255));
        ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);

        static bool updateExpr = false;
        if (ImPlot::IsPlotHovered())
        {
            static auto prevPos = ImPlotPoint(0, 0);
            auto pos = ImPlot::GetPlotLimits().Min(); 

            if (prevPos.x != pos.x || prevPos.y != pos.y)
            {
                updateExpr = true;
            }
            
            prevPos = pos;
        }
        
        // Draw our functions 
        for (auto expr : kubvc::math::expressions::Expressions)
        {                    
            if (expr != nullptr)
            {
                if (expr->isShowing() && expr->isValid())
                { 
                    if (updateExpr)
                    {
                        updateExpressionByPlotLimits(expr);        
                    }

                    auto buffer = expr->getPlotBuffer(); 
                    if (buffer.size() > 0)
                    {
                        // Apply plot style from expression                                                   
                        ImPlot::SetNextLineStyle(kubvc::render::toImVec4(expr->Settings.color), expr->Settings.thickness);

                        static constexpr auto stride = 2 * sizeof(double);

                        const auto shaded = expr->Settings.shaded ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                        const auto plotLineFlags = ImPlotLineFlags_::ImPlotLineFlags_NoClip | shaded;

                        ImPlot::PlotLine(expr->getTextBuffer().data(), &buffer[0].x, &buffer[0].y, buffer.size(), plotLineFlags, 0, stride);      
                        //ImPlot::PlotScatter(expr->textBuffer.data(), &expr->plotBuffer[0].x, &expr->plotBuffer[0].y, expr->plotBuffer.size(), plotLineFlags, 0, stride);      
                    }
                }
            }    
        }                        
        updateExpr = false;
        ImPlot::EndPlot();
    }
}

static bool drawPickElementButton(const std::string& text, const ImVec2& size)
{
    auto cText = text.c_str();

    if (ImGui::Button(cText, size))
    {
        auto selected = kubvc::math::expressions::Selected;
        if (selected != nullptr)
        {
            const auto len = std::strlen(cText);
            auto end = cText + len;
            auto& buffer = selected->getTextBuffer();
            // Find last empty character in buffer 
            auto beg = buffer.begin() + selected->getCursor();
            
            buffer.insert(beg, cText, end);
            
            selected->setCursor(selected->getCursor() + len);        
            selected->parseAndEval();
        }

        return true;
    }    

    return false;
}

static void drawOperatorsKeyboard()
{
    const auto opColumnsCount = 6;
    ImGui::TextDisabled("Operators");
    ImGui::Separator();

    if (ImGui::BeginTable("opTable", opColumnsCount))
    {                        
        static const std::initializer_list<unsigned char> ops = { '+', '-', '*', '/', '^', '=' };
        const auto opButtonSize = ImVec2(35.0f, 35.0f);
        for (auto item : ops)
        {         
            ImGui::TableNextColumn();       
            drawPickElementButton(std::string(1, item), opButtonSize);                
        }
        
        ImGui::TableNextColumn();       
        drawPickElementButton(std::string(1, '('), opButtonSize);                

        ImGui::TableNextColumn();       
        drawPickElementButton(std::string(1, ')'), opButtonSize);                
        
        ImGui::EndTable();
    }
}

static void drawKeysKeyboard()
{
    const auto opColumnsCount = 6;
    const auto opButtonSize = ImVec2(35.0f, 35.0f);        
    ImGui::TextDisabled("Keys");
    ImGui::Separator(); 
    static bool isUp = false;    
    if (ImGui::Button("Up", opButtonSize))
    {
        isUp = !isUp;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("<-", opButtonSize))
    {
        auto selected = kubvc::math::expressions::Selected;
        auto cursor = selected->getCursor();
        
        if (cursor > 0)
        {
            // Remove character by cursor from text buffer 
            auto& buffer = selected->getTextBuffer();
            auto begin = buffer.begin() + cursor;
            buffer.erase(begin - 1, begin);

            selected->setCursor(cursor - 1);
            
            // Update  
            selected->parseAndEval();
        }
    }

    if (ImGui::BeginTable("keysTable", opColumnsCount))
    {                        
        // Very, a very dumb way to implement qwerty keyboard
        // Maybe not, idk
        const std::int8_t QWERTY_KEYS_SIZE = 26; 
        const char* QWERTY_KEYS_UP = "QWERTYUIOPASDFGHJKLZXCVBNM";
        const char* QWERTY_KEYS_DOWN = "qwertyuiopasdfghjklzxcvbnm";
        
        for (std::uint8_t i = 0; i < QWERTY_KEYS_SIZE; i++)
        {         
            ImGui::TableNextColumn();       
            drawPickElementButton(std::string(1, isUp ? QWERTY_KEYS_UP[i] : QWERTY_KEYS_DOWN[i]), opButtonSize);                
        }
        
        ImGui::EndTable();
    }
}

static void drawNumbersKeyboard()
{
    const auto opColumnsCount = 6;
    ImGui::TextDisabled("Numbers");
    ImGui::Separator();

    if (ImGui::BeginTable("numTable", opColumnsCount))
    {                        
        const auto opButtonSize = ImVec2(35.0f, 35.0f);
        for (char i = '0'; i <= '9'; i++)
        {         
            ImGui::TableNextColumn();       
            drawPickElementButton(std::string(1, i), opButtonSize);                
        }
        
        ImGui::EndTable();
    }

    ImGui::TextDisabled("Constants");
    ImGui::Separator();

    if (ImGui::BeginTable("constTable", opColumnsCount))
    {                        
        const auto opButtonSize = ImVec2(35.0f, 35.0f);
        for (auto i : kubvc::math::containers::Constants)
        {         
            ImGui::TableNextColumn();       
            drawPickElementButton(i.first, opButtonSize);                
        }
        
        ImGui::EndTable();
    }
}

static void drawFunctionsKeyboard()
{
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::TextDisabled("Functions");
    ImGui::Separator();

    const auto funcColumnsCount = 5;
    if (ImGui::BeginTable("funcTable", funcColumnsCount))
    {     
        std::int32_t itemCount = 0;                   
        for (auto item : kubvc::math::containers::Functions)
        {         
            if (itemCount % funcColumnsCount == 0 && itemCount != 0)
            {
                ImGui::TableNextRow();
            }

            ImGui::TableNextColumn();       
            if (drawPickElementButton(item.first, ImVec2(0.0f, 35.0f)))
            {
                ImGui::CloseCurrentPopup();
            }

            ++itemCount;
        }
        ImGui::EndTable();
    }

    if (ImGui::Button("Close"))
    {
        ImGui::CloseCurrentPopup();
    }
}

int main()
{
    // Initialize main application components
    const auto window = kubvc::application::Window::getInstance();
    window->createWindow(1920, 1080, 0, 0, "KubVC");
    
    const auto render = kubvc::render::Renderer::getInstance();
    render->init();

    const auto gui = kubvc::render::GUI::getInstance();
    gui->init();

    // Run main loop 
    while (!window->shouldClose())
    {
        render->clear();
        // TODO: imgui wrap  
        gui->begin();
        gui->beginDockspace();
        {
            const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
            const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding;
            // TODO: Could be closed
            if (ImGui::Begin("Keyboard"))
            {
                // TODO: Operators, functions as childs
                if (ImGui::Button("Functions"))
                {
                    ImGui::OpenPopup("FunctionsKeyboardPopup");
                }

                if (ImGui::BeginPopup("FunctionsKeyboardPopup"))
                { 
                    drawFunctionsKeyboard();
                    ImGui::EndPopup();
                }

                static const auto keyboardTableFlags = ImGuiTableFlags_::ImGuiTableFlags_Reorderable | ImGuiTableFlags_::ImGuiTableFlags_Hideable 
                    | ImGuiTableFlags_::ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                    | ImGuiTableFlags_::ImGuiTableFlags_RowBg |  ImGuiTableFlags_::ImGuiTableFlags_Resizable
                    | ImGuiTableFlags_::ImGuiTableFlags_PadOuterX;
                if (ImGui::BeginTable("##KeyboardTable", 4, keyboardTableFlags))
                {                    
                    ImGui::TableNextColumn();
                    if (ImGui::BeginChild("NumbersKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags))
                    { 
                        drawNumbersKeyboard();
                    }
                    ImGui::EndChild();

                    ImGui::TableNextColumn();
                    if (ImGui::BeginChild("OperatorsKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags))
                    { 
                        drawOperatorsKeyboard();
                    }
                    ImGui::EndChild();

                    ImGui::TableNextColumn();
                    if (ImGui::BeginChild("keysKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags))
                    { 
                        drawKeysKeyboard();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndTable();
            }
            ImGui::End();

            
            ImGui::SameLine();
            if (ImGui::Begin("Graph List"))
            {
                auto windowSize = ImGui::GetWindowSize();
                if (ImGui::BeginChild("MainGraphPanel", ImVec2(windowSize.x - 15.0f, 40.0f), childFlags))
                {
                    drawMainGraphPanel();
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
            ImGui::End();

            if (ImGui::Begin("GraphOptions"))
            { 
                ImGui::TextDisabled("Current graph settings");
                ImGui::Separator();      
                auto selected = kubvc::math::expressions::Selected;
                if (selected != nullptr)
                {          
                    ImGui::TextDisabled("Graph: %s", selected->getTextBuffer().data());
                    ImGui::Dummy(ImVec2(0, 15.0f));
                    
                    ImGui::Text("Visible");
                    ImGui::SameLine();

                    bool visible = selected->isShowing();
                    if (ImGui::Checkbox("##OptionsGraphVisibleCheckBox", &visible))
                    {
                        selected->setShow(visible);
                    }
                    
                    ImGui::Text("Shaded");
                    ImGui::SameLine();
                    ImGui::Checkbox("##OptionsGraphShadedCheckBox", &selected->Settings.shaded);

                    // Do not show color editor when we are not generate random color    
                    if (selected->Settings.isRandomColorSetted)
                    {
                        ImGui::Text("Color");
                        ImGui::SameLine();

                        auto color = kubvc::render::toImVec4(selected->Settings.color);
                        if (ImGui::ColorButton("##OptionsGraphColorPicker", color))
                        {
                            selected->Settings.color = kubvc::render::toGlmVec4(color);
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
            ImGui::End();

            if (ImGui::Begin("Viewer"))
            {           
                drawPlotter();
            }

            ImGui::End();  

            const auto fpsCounterWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking 
                | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize 
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse; 

            if (ImGui::Begin("##FpsCounter", nullptr, fpsCounterWindowFlags))
            {           
                auto io = ImGui::GetIO();
                ImGui::Text("Fps %.1f", io.Framerate);
            }
            
            ImGui::End();       
        }
        

        gui->endDockspace();
        gui->end();
        window->swapAndPool();
    }
    
    kubvc::math::expressions::Expressions.clear();
    kubvc::math::expressions::Expressions.shrink_to_fit();

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
