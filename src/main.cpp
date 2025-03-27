#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "ast.h"
#include "gui.h"
#include "alg_helpers.h"

#include <algorithm>


static inline void toLowerCase(std::string& text)
{
    std::transform(text.begin(), text.end(), text.begin(), std::tolower);
}

static inline std::string getTypeName(const kubvc::algorithm::NodeTypes& type)
{
    switch (type)
    {
        case kubvc::algorithm::NodeTypes::None:
            return "None";           
        case kubvc::algorithm::NodeTypes::Root:
            return "Root";           
        case kubvc::algorithm::NodeTypes::Number: 
            return "Number";           
        case kubvc::algorithm::NodeTypes::Variable:
            return "Variable";           
        case kubvc::algorithm::NodeTypes::Function:
            return "Function";           
        case kubvc::algorithm::NodeTypes::Operator:
            return "Operator";               
    }

    return "Unknown";
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

static auto createVariableNode(const kubvc::algorithm::ASTree& tree, char value)
{
    auto varNode = tree.createNode<kubvc::algorithm::VariableNode>();
    varNode->value = value;    
    return varNode;
}

static auto createNumberNode(const kubvc::algorithm::ASTree& tree, double value)
{
    auto numNode = tree.createNode<kubvc::algorithm::NumberNode>();
    numNode->value = value;    
    return numNode;
}

static auto createOperator(const kubvc::algorithm::ASTree& tree, std::shared_ptr<kubvc::algorithm::Node> x, 
    std::shared_ptr<kubvc::algorithm::Node> y, char op)
{
    auto opNode = tree.createNode<kubvc::algorithm::OperatorNode>();
    opNode->operation = op;
    opNode->left = std::move(x);
    opNode->right = std::move(y);
    return opNode;
}

static auto createUnaryOperator(const kubvc::algorithm::ASTree& tree, std::shared_ptr<kubvc::algorithm::Node> x, char op)
{
    auto opNode = tree.createNode<kubvc::algorithm::UnaryOperatorNode>();
    opNode->operation = op;
    opNode->child = std::move(x);
    return opNode;
}


static auto createInvalid(const kubvc::algorithm::ASTree& tree, const std::string& name)
{
    auto invalidNode = tree.createNode<kubvc::algorithm::InvalidNode>();
    invalidNode->name = name;
    return invalidNode;
}

static auto createFunction(const kubvc::algorithm::ASTree& tree, const std::string& name)
{
    auto funcNode = tree.createNode<kubvc::algorithm::FunctionNode>();
    funcNode->name = name;

    return funcNode;
}

static inline unsigned char getCurrentChar(const std::size_t& cursor, const std::string& text) 
{
    if (cursor > text.size())
    {
        FATAL("Cursor is out of bounds");
        return '\0';
    }
    auto character = text[cursor];

    return static_cast<unsigned char>(character);
}

static auto parseNumbers(std::size_t& cursor, const std::string& text)
{
    char character = getCurrentChar(cursor, text);
    std::string output = std::string();
    while(kubvc::algorithm::Helpers::isDigit(character) 
        || kubvc::algorithm::Helpers::isDot(character))
    {
        output += character;
        cursor++;
        character = getCurrentChar(cursor, text);
    }
    return output;
}

static auto parseLetters(std::size_t& cursor, const std::string& text, bool includeDigits = true)
{
    char character = getCurrentChar(cursor, text);
    std::string output = std::string();
    while(kubvc::algorithm::Helpers::isLetter(character) || (kubvc::algorithm::Helpers::isDigit(character) && includeDigits))
    {
        output += character;
        cursor++;
        character = getCurrentChar(cursor, text);
    }
    return output;
}

static std::shared_ptr<kubvc::algorithm::Node> parseExpression(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, bool isSubExpression);
static std::shared_ptr<kubvc::algorithm::Node> parseFunction(const kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, std::size_t& cursor, const std::string& text)
{
    std::size_t funcCursor = cursor_pos;
    std::string funcName = std::string();

    funcName = parseLetters(funcCursor, text);

    // Convert text to lower case to avoid mismatch 
    toLowerCase(funcName);

    if (funcCursor > text.size())
        return createInvalid(tree, text);
  
    // Next should be bracket character
    auto brChar = getCurrentChar(funcCursor, text);        

    // TODO: What if we are want support functions with more than one argument
    if (kubvc::algorithm::Helpers::isBracketStart(brChar))
    {
        DEBUG("So, is bracket found...");
        cursor++;
    
        auto argsNode = parseExpression(tree, text, cursor, true);
        if (argsNode->getType() != kubvc::algorithm::NodeTypes::Invalid)
        {
            auto funcNode = createFunction(tree, funcName);
            funcNode->argument = argsNode;
            return funcNode;            
        }
        
        WARN("Bad node returned...");
    }

    return createInvalid(tree, text);
}


static std::shared_ptr<kubvc::algorithm::Node> parseElement(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, char currentChar, bool isSubExpression)
{
    DEBUG("try to find something usefull for %c", currentChar); 

    // Skip white space if we are find it  
    if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar))
    {
        DEBUG("Ignore white space in parse element stage"); 
        cursor++;
        currentChar = getCurrentChar(cursor, text);
    } 

    if (kubvc::algorithm::Helpers::isDigit(currentChar))
    {
        auto out = parseNumbers(cursor, text);

        DEBUG("Is digit %s", out.c_str());
        if (out.empty())
        {
            ERROR("Parse number has a empty output, hmm. Ignore!");
        }
        else
        {
            if (kubvc::algorithm::Helpers::isNumber(out))
            {
                return createNumberNode(tree, std::atof(out.c_str()));                
            }
            else
            {
                ERROR("Output is actually not a number, so ignore him! We catch that bad guy: %s", out.c_str());
            }
        }
    }    
    else if (kubvc::algorithm::Helpers::isLetter(currentChar))
    {
        auto out = parseLetters(cursor, text);
        DEBUG("Is letter | parsed %s", out.c_str());
        const auto outSize = out.size();
        if (outSize == 0)
        {
            ERROR("Output has a zero size");                
        } 
        else if (outSize > 1)
        {
            DEBUG("Parse function...");
            return parseFunction(tree, cursor - outSize, cursor, text);
        }
        else 
        {
            // TODO: What we need to do with constants 
            return createVariableNode(tree, currentChar);
        }
    }
    else if (kubvc::algorithm::Helpers::isBracketStart(currentChar)) 
    {
        cursor++;
        DEBUG("Bracket start");
        auto node = parseExpression(tree, text, cursor, true);
        return node;
    } 
    else if (kubvc::algorithm::Helpers::isUnaryOperator(currentChar))
    {
        DEBUG("Possible unary operator");
        cursor++;
        auto node = parseExpression(tree, text, cursor, isSubExpression);

        cursor--;
        return createUnaryOperator(tree, node, currentChar);
    }

    DEBUG("Nothing found");

    return createInvalid(tree, "INV_NODE");
}

static std::shared_ptr<kubvc::algorithm::Node> parseExpression(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, bool isSubExpression)
{
    // Don't do anything if text is empty 
    if (text.size() == 0)
        return nullptr;
    
    DEBUG("----------------------------------------");
    DEBUG("parseExpression | Start | cursor: %d", cursor);
    
    std::shared_ptr<kubvc::algorithm::Node> left = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);

    while (true)
    {
        auto currentChar = getCurrentChar(cursor, text);  
        DEBUG("Current character in expression cycle: %c", currentChar); 

        // Ignore white spaces
        if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar))
        {
            DEBUG("Ignore white space in expression parse"); 
            cursor++;
            continue;
        }  
        // We are want to continue cycle or want to break it if it's a subexpression
        else if (kubvc::algorithm::Helpers::isBracketEnd(currentChar))
        {
            DEBUG("End of bracket | isSubExpression:%i", isSubExpression);
            cursor++;

            if (isSubExpression)
            {
                DEBUG("Return left node");
                return left;
            }

            continue;
        }
        // If current character is not operator we are leave from cycle 
        else if (!kubvc::algorithm::Helpers::isOperator(currentChar))
        {
            DEBUG("parseExpression | End | Leave from cycle");
            DEBUG("----------------------------------------");
            break;
        }

        // Augment cursor position 
        cursor++;  
        // Try to find something 
        auto right = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);
        if (right == nullptr)
        {
            ERROR("parseElement is returned nullptr, maybe syntax error or not implemented element");
            return nullptr;
        }

        left = createOperator(tree, left, right, currentChar);
    }   

    // If we are actually leave from cycle and if isSubExpression is true, 
    // it means we are not found end brecket symbol, so it's a invalid expression   
    if (isSubExpression)
    {
        // In some cases we are reached from text range by one character, so it can be end of bracket. 
        if (cursor > text.size())
        {
            WARN("Edgy case found");
            auto preLastChar = getCurrentChar(cursor - 1, text);
            if (kubvc::algorithm::Helpers::isBracketEnd(preLastChar))
            {
                return left;    
            }
        }
        
        WARN("is invalid brecket");
        return createInvalid(tree, "InvalidBrecket");
    }

    return left;
}

static void parse(const kubvc::algorithm::ASTree& tree, const std::string& text, const std::size_t cursor_pos = 0)
{
    std::size_t cursor = cursor_pos;
    auto root = static_cast<kubvc::algorithm::RootNode*>(tree.getRoot().get());
    root->child = parseExpression(tree, text, cursor, false);
}

static void createTree(kubvc::algorithm::ASTree& tree)
{ 
    tree.clear();
    tree.makeRoot();
}

static const auto MAX_FUNC_RANGE = 100;
static const auto MAX_BUFFER_SIZE = 1024;
static const auto MAX_PLOT_BUFFER_SIZE = 2048;

struct Expression
{
    bool show = true;
    kubvc::algorithm::ASTree tree = { };
    std::int32_t id = -1;
    std::vector<char> textBuffer = std::vector<char>(MAX_BUFFER_SIZE);  
    std::vector<glm::dvec2> plotBuffer = std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE);
   
    ImVec4 plotLineColor;
    
    float thickness = 2.0f;

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

static void calculatePlotPoints(std::shared_ptr<Expression> expr, double max = MAX_FUNC_RANGE, double min = -MAX_FUNC_RANGE)
{    
    auto root = expr->tree.getRoot();
    if (root->child == nullptr)
        return;

    auto getRanges = [&](const kubvc::algorithm::ASTree& tree, double& xMax, double& xMin)
    {
        double yMin = INFINITY;
        double yMax = -INFINITY;
        
        for (std::int32_t i = 0; i < MAX_PLOT_BUFFER_SIZE; ++i)
        {
            double result = 0.0;
            auto lerpAxis = std::lerp(yMin, yMax, static_cast<double>(i) / (MAX_PLOT_BUFFER_SIZE - 1));
            root->calculate(lerpAxis, result);
            yMin = std::min(yMin, result);
            yMax = std::max(yMax, result);
        }

        if (yMin < - 1e6 || yMax > 1e6)
        {
            xMin *= 0.5;
            xMax *= 0.5;
        } 
        else if (std::abs(yMin) < 1e-6 
            && std::abs(yMax) < 1e-6)
        {
            xMin *= 2.0;
            xMax *= 2.0;
        }
    };

    double xMax = max;
    double xMin = min;

    for (std::int32_t i = 0; i < MAX_PLOT_BUFFER_SIZE; ++i)
    {
        double result = 0.0;
        auto lerpAxis = std::lerp(xMin, xMax, static_cast<double>(i) / (MAX_PLOT_BUFFER_SIZE - 1));
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

static void drawFunctionInList(std::shared_ptr<Expression> expr, const std::int32_t& id, const std::int32_t& index)
{
    auto idStr = std::to_string(id);
    
    ImGui::Text("%d:", index);
    ImGui::SameLine();

    // Set special color for textbox border when we are selected expression
    if (expr == selectedExpression)
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, ImVec4(0.64f, 0.16f, 0.16f, 1.0f));

    if (ImGui::InputText(("##" + idStr + "_ExprInputText").c_str(), expr->textBuffer.data(), expr->textBuffer.size()))
    {
        parse(expr->tree, expr->textBuffer.data());
        calculatePlotPoints(expr);
    }
    // Revert color changes
    ImGui::PopStyleColor(static_cast<std::int32_t>(expr == selectedExpression));
    
    // Set current expression by clicking on textbox 
    if (ImGui::IsItemActive() && ImGui::IsItemClicked())
    {
        selectedExpression = expr;
    }

    ImGui::SameLine();

    // Do not show color editor when we are not generate random color    
    if (expr->isRandomColorSetted)
    {
        if (ImGui::ColorButton(("##" + idStr + "_ExprColorPicker").c_str(), expr->plotLineColor))
        {
            expr->changeColor = !expr->changeColor;
            selectedExpression = expr;
        }
    }
    
    ImGui::SameLine();
    ImGui::PushItemWidth(45.0f);
    ImGui::DragFloat(("##" + idStr + "_ExprThick").c_str(), &expr->thickness, 0.1f, 0.5f, 10.0f);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
    if (ImGui::Button("-"))
    {
        auto it = std::find_if(expressions.begin(), expressions.end(), [expr](auto it) { return it->id == expr->id; });
        if (it == expressions.end())
        {
            ERROR("WTF: it == expressions.end()");
        }
        else
        {
            expressions.erase(it);
        }
    }
    ImGui::PopID();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Remove this graph from graph list");
    }

    ImGui::SameLine();

    ImGui::PushID(("##" + idStr + "_ExprRadioButton").c_str());    
    if (ImGui::RadioButton("V", expr->show))
    {
        expr->show = !expr->show;
    }
    ImGui::PopID();
    
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Change visibility for this graph.");
    }
}

static void drawAddExpressionButton()
{
    if (ImGui::Button("+"))
    {
        auto expr = std::make_shared<Expression>();
        createTree(expr->tree);
        
        // Dummy id set
        static std::int32_t id = 0;
        id++;
        expr->id = id;
        expressions.push_back(expr);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("A button which you can add new graph.");
    }
}

static void drawExpressionsList()
{
    std::int32_t expressionIndex = 0;
    for (auto expr : expressions)
    {
        if (expr != nullptr)
        {
            expressionIndex++;
            drawFunctionInList(expr, expr->id, expressionIndex);
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
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Show a abstract syntax tree for current graph. Only for debug purposes");
        }
    }
}

static void drawColorEditor()
{
    if (selectedExpression != nullptr && selectedExpression->changeColor)
    {
        ImGui::ColorPicker4("##_CurrentExprColorPicker", &selectedExpression->plotLineColor.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel);
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
            // TODO: Convert function into gui class  
            if (ImGui::Begin("Toolbox"))
            {
                drawAddExpressionButton();
                ImGui::Separator();
                drawExpressionsList();
                drawColorEditor();
                ImGui::Separator();
                drawDebugAST();
              

            }
            ImGui::End();

            if (ImGui::Begin("Viewer"))
            {           
                auto size = ImGui::GetContentRegionAvail();
                const auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;

                if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) 
                {	
                    ImPlot::SetupAxis(ImAxis_X1, "X-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
                    ImPlot::PushStyleColor(ImPlotCol_::ImPlotCol_AxisBgActive, ImVec4(255,0,0,255));
                    ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
                    
                    for (auto expr : expressions)
                    {                    
                        if (expr != nullptr)
                        {
                            // TODO: Legend can change visibility too 
                            if (expr->show == true)
                            {                                                    
                                ImPlot::SetNextLineStyle(expr->plotLineColor, expr->thickness);
                                ImPlot::PlotLine(expr->textBuffer.data(), &expr->plotBuffer[0].x, &expr->plotBuffer[0].y, expr->plotBuffer.size(), 0, 0, 2 * sizeof(double));      
                            }
                        }    
                    }                        
                    ImPlot::EndPlot();
                }
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
