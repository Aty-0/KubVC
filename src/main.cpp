#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "gui.h"
#include "expression_parser.h"
#include <iterator>

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


static void createTree(kubvc::algorithm::ASTree& tree)
{ 
    tree.clear();
    tree.createRoot();
}

static const auto MAX_FUNC_RANGE = 100;
static const auto MAX_BUFFER_SIZE = 1024;
static const auto MAX_PLOT_BUFFER_SIZE = 2048;

struct Expression
{
    bool show = true;
    std::int32_t id = -1;
    
    std::int32_t cursor = 0;
    std::vector<char> textBuffer = std::vector<char>(MAX_BUFFER_SIZE);  
    
    kubvc::algorithm::ASTree tree = { };
    std::vector<glm::dvec2> plotBuffer = std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE);
   
    ImVec4 plotLineColor;
    
    float thickness = 2.0f;

    bool valid = true;

    bool shaded = false;
    bool changeColor;
    bool isRandomColorSetted;    

    ~Expression()
    {
        DEBUG("Destroy expression id %d ...", id);

        show = false;

        tree.clear();

        id = -1;

        textBuffer.clear();
        textBuffer.shrink_to_fit();

        plotBuffer.clear();
        plotBuffer.shrink_to_fit();
    }
};

static std::vector<std::shared_ptr<Expression>> expressions = { };  
static std::shared_ptr<Expression> selectedExpression = nullptr;

static void calculatePlotPoints(std::shared_ptr<Expression> expr, double max, double min, std::int32_t pointsDetail = MAX_PLOT_BUFFER_SIZE)
{    
    auto root = expr->tree.getRoot();
    if (root->child == nullptr)
        return;

    for (std::int32_t i = 0; i < pointsDetail; ++i)
    {
        double result = 0.0;
        auto lerpAxis = std::lerp(min, max, static_cast<double>(i) / (pointsDetail - 1));
        root->calculate(lerpAxis, result);
        expr->plotBuffer[i] = { lerpAxis, result };
    }
 
    if (!expr->isRandomColorSetted)
    {
        std::srand(std::time({}));  
        expr->plotLineColor = { 0.01f * (std::rand() % 255), 0.01f * (std::rand() % 255), 0.01f * (std::rand() % 255), 1.0f };
        expr->isRandomColorSetted = true;
    }
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
    
    auto expr = *static_cast<std::shared_ptr<Expression>*>(data->UserData);
    expr->cursor = data->CursorPos;

    return 0;
}

static void drawEditGraph(kubvc::render::GUI* gui, std::shared_ptr<Expression> expr, const std::int32_t& id, const std::int32_t& index)
{
    auto idStr = std::to_string(id);
    
    ImGui::PushFont(gui->getMathFont());
    ImGui::Text("%d:", index);
    ImGui::SameLine();
    // Set special color for textbox border when we are selected expression or get invalid node somewhere kekw
    if (!expr->valid)
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, INVALID_COLOR);
    else if (expr == selectedExpression)
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, SELECTED_COLOR);

    if (ImGui::InputText(("##" + idStr + "_ExprInputText").c_str(), expr->textBuffer.data(), expr->textBuffer.size(), 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, handleExpressionCursorPosCallback, &expr))
    {
        kubvc::algorithm::Parser::parse(expr->tree, expr->textBuffer.data());
        calculatePlotPoints(expr, MAX_FUNC_RANGE, -MAX_FUNC_RANGE);    
        expr->valid = expr->tree.isValid();
    }

    ImGui::PopFont();
    
    // Revert color changes
    auto popColor = static_cast<std::int32_t>(expr == selectedExpression || !expr->valid);
    ImGui::PopStyleColor(popColor);

    static const auto fontBig = gui->getDefaultFontMathSize();

    // Set current expression by clicking on textbox 
    if (ImGui::IsItemActive() && ImGui::IsItemClicked())
    {
        selectedExpression = expr;
    }

    ImGui::SameLine();

    ImGui::PushFont(fontBig);
    ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
    if (ImGui::Button("-"))
    {
        auto it = std::find_if(expressions.begin(), expressions.end(), [expr](auto it) { return it->id == expr->id; });
        if (it != expressions.end())
        {
            // Set as nullptr to avoid some weird behaviour
            if (selectedExpression == expr)
            {
                selectedExpression = nullptr;
            }

            expressions.erase(it);
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
    if (ImGui::RadioButton("V", expr->show))
    {
        expr->show = !expr->show;
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
        auto expr = std::make_shared<Expression>();
        createTree(expr->tree);
        
        // Dummy id set
        static std::int32_t id = 0;
        id++;
        expr->id = id;
        expressions.push_back(expr);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("A button which you can add new graph.");
    }

    ImGui::SameLine(region.x - 55.0f);
    ImGui::SetNextItemWidth(region.x - 55.0f);
    
    if (ImGui::Button("Clear All"))
    {
        selectedExpression = nullptr;
        expressions.clear();
        expressions.shrink_to_fit();
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
    for (auto expr : expressions)
    {
        if (expr != nullptr)
        {
            expressionIndex++;
            drawEditGraph(gui, expr, expr->id, expressionIndex);
        }
    }
}

static void drawDebugAST()
{
    if (ImGui::CollapsingHeader("AST debug"))
    {
        if (selectedExpression != nullptr)
        {
            ImGui::Text("AST:");   
            ImGui::Text("Current tree is %s", selectedExpression->textBuffer.data());   
            
            static bool listStyleTree = false;
            ImGui::Checkbox("List style for tree", &listStyleTree);
            if (listStyleTree)
            {
                showTreeList(selectedExpression->tree.getRoot());
            }
            else
            {
                showTreeVisual(selectedExpression->tree);
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
    if (selectedExpression != nullptr && selectedExpression->changeColor)
    {
        ImGui::ColorPicker4("##_CurrentExprColorPicker", &selectedExpression->plotLineColor.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);
    }
}

static void updateExpressionByPlotLimits(std::shared_ptr<Expression> expr)
{
    auto limits = ImPlot::GetPlotLimits();                 
    calculatePlotPoints(expr, limits.X.Max, limits.X.Min);
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
        for (auto expr : expressions)
        {                    
            if (expr != nullptr)
            {
                if (expr->show == true && expr->valid)
                { 
                    if (updateExpr)
                    {
                        updateExpressionByPlotLimits(expr);        
                    }
                    // Apply plot style from expression                                                   
                    ImPlot::SetNextLineStyle(expr->plotLineColor, expr->thickness);
                    
                    static constexpr auto stride = 2 * sizeof(double);
                    
                    const auto shaded = expr->shaded ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                    const auto plotLineFlags = ImPlotLineFlags_::ImPlotLineFlags_SkipNaN 
                        | shaded;

                    ImPlot::PlotLine(expr->textBuffer.data(), &expr->plotBuffer[0].x, &expr->plotBuffer[0].y, expr->plotBuffer.size(), plotLineFlags, 0, stride);      
                }
            }    
        }                        
        updateExpr = false;
        ImPlot::EndPlot();
    }
}

static void drawPickElementButton(const std::string& text, const ImVec2& size)
{
    auto cText = text.c_str();

    if (ImGui::Button(cText, size))
    {
        if (selectedExpression != nullptr)
        {
            auto end = cText + std::strlen(cText);
            auto& buffer = selectedExpression->textBuffer;
            // Find last empty character in buffer 
            auto beg = buffer.begin() + selectedExpression->cursor;
            
            buffer.insert(beg, cText, end);
        }
    }    
}

static void drawToolsTable()
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
            drawPickElementButton(item.first, ImVec2(0.0f, 35.0f));

            ++itemCount;
        }
        ImGui::EndTable();
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
        gui->begin();
        gui->beginDockspace();
        {
            const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
            const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar;

            // TODO: Convert function into gui class  
            if (ImGui::Begin("Tools"))
            {
                ImGui::Text("Tools list:");
                ImGui::Separator();
                if (ImGui::BeginChild("FunctionsListChild", ImVec2(0, 0), childFlags, childWindowFlags))
                { 
                    drawToolsTable();
                }
                ImGui::EndChild();
            }
            ImGui::End();
            
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
                
                if (selectedExpression != nullptr)
                {          
                    ImGui::TextDisabled("Graph: %s", selectedExpression->textBuffer.data());
                    ImGui::Dummy(ImVec2(0, 15.0f));
                    
                    ImGui::Text("Visible");
                    ImGui::SameLine();
                    ImGui::Checkbox("##OptionsGraphVisibleCheckBox", &selectedExpression->show);
                    
                    ImGui::Text("Shaded");
                    ImGui::SameLine();
                    ImGui::Checkbox("##OptionsGraphShadedCheckBox", &selectedExpression->shaded);
                    // Do not show color editor when we are not generate random color    
                    if (selectedExpression->isRandomColorSetted)
                    {
                        ImGui::Text("Color");
                        ImGui::SameLine();
                        if (ImGui::ColorButton("##OptionsGraphColorPicker", selectedExpression->plotLineColor))
                        {
                            selectedExpression->changeColor = !selectedExpression->changeColor;
                            selectedExpression = selectedExpression;
                        }
                    }

                    ImGui::Text("Line Thickness");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(45.0f);
                    if (ImGui::DragFloat("##OptionsGraphThicknessDrag", &selectedExpression->thickness, THICKNESS_SPEED, THICKNESS_MIN, THICKNESS_MAX, "%.1f"))
                    {
                        // Handle manualy writed value
                        if (selectedExpression->thickness > THICKNESS_MAX)
                        {
                            selectedExpression->thickness = THICKNESS_MAX;
                        } 
                        else if (selectedExpression->thickness < THICKNESS_MIN)
                        {
                            selectedExpression->thickness = THICKNESS_MIN;
                        }     
                    }
                    ImGui::PopItemWidth();


                    drawLineColorPicker();
                    
                    ImGui::Separator();
                    drawDebugAST();
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
    
    expressions.clear();
    expressions.shrink_to_fit();

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
