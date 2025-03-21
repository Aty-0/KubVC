#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "ast.h"
#include "gui.h"
#include "alg_helpers.h"

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
            ERROR("Unknown type or not implemented");
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
                switch (node->argument->getType())
                {
                    case kubvc::algorithm::NodeTypes::Number:
                    {
                        auto numberArg = std::dynamic_pointer_cast<kubvc::algorithm::NumberNode>(node->argument);        
                        // FIXME: why %f is showing 0 need investigate and remove std::to_string            
                        ImGui::Text("(%s)", std::to_string(numberArg->value).c_str());
                        break;
                    }   
                    case kubvc::algorithm::NodeTypes::Variable:
                    {
                        auto numberArg = std::dynamic_pointer_cast<kubvc::algorithm::VariableNode>(node->argument);                    
                        ImGui::Text("(%s)", numberArg->value.c_str());
                        break;
                    }   
                }
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
            ERROR("Unknown type or not implemented");
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

static auto createExpression(const kubvc::algorithm::ASTree& tree, std::shared_ptr<kubvc::algorithm::Node> x, 
    std::shared_ptr<kubvc::algorithm::Node> y, char op)
{
    auto opNode = tree.createNode<kubvc::algorithm::OperatorNode>();
    opNode->operation = op;
    opNode->left = std::move(x);
    opNode->right = std::move(y);
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

static auto parseLetters(std::size_t& cursor, const std::string& text)
{
    char character = getCurrentChar(cursor, text);
    std::string output = std::string();
    while(kubvc::algorithm::Helpers::isLetter(character))
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
    char character = getCurrentChar(funcCursor, text);
    std::string funcName = std::string();
    while(kubvc::algorithm::Helpers::isLetter(character))
    {
        funcName += character;
        funcCursor++;
        character = getCurrentChar(funcCursor, text);        
    } 
    
    if (funcCursor >= text.size())
        return createInvalid(tree, text);
  
    character = getCurrentChar(funcCursor, text);        

    // TODO: It's kinda wrong implementation, but it's fine for now...
    if (kubvc::algorithm::Helpers::isBracketStart(character))
    {
        cursor++;
        auto argsNode = parseExpression(tree, text, cursor, true);
        switch (argsNode->getType())
        {
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
            {
                auto funcNode = createFunction(tree, funcName);
                funcNode->argument = argsNode;
                return funcNode;
            }
        }
    }

    return createInvalid(tree, text);
}


static std::shared_ptr<kubvc::algorithm::Node> parseElement(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, char currentChar)
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

    DEBUG("Nothing found");

    return createInvalid(tree, "INV_NODE");
}

static std::shared_ptr<kubvc::algorithm::Node> parseExpression(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, bool isSubExpression)
{
    // Don't do anything if text is empty 
    if (text.size() == 0)
        return nullptr;

    DEBUG("parseExpression | cursor: %d", cursor);
    std::shared_ptr<kubvc::algorithm::Node> left = parseElement(tree, text, cursor, getCurrentChar(cursor, text));

    while (true)
    {
        auto character = getCurrentChar(cursor, text);  
        DEBUG("Current character in expression cycle: %c", character); 

        // Ignore white spaces
        if (kubvc::algorithm::Helpers::isWhiteSpace(character))
        {
            DEBUG("Ignore white space in expression parse"); 
            cursor++;
            continue;
        }  
        // We are want to continue cycle or want to break it if it's a subexpression
        else if (kubvc::algorithm::Helpers::isBracketEnd(character))
        {
            DEBUG("End of bracket");
            cursor++;

            if (isSubExpression)
                return left;

            continue;
        }
        // If current character is not operator we are leave from cycle 
        else if (!kubvc::algorithm::Helpers::isOperator(character))
        {
            DEBUG("Leave from cycle");
            break;
        }

        // Augment cursor position 
        cursor++;  
        // Try to find something 
        auto right = parseElement(tree, text, cursor, getCurrentChar(cursor, text));
        if (right == nullptr)
        {
            ERROR("parseElement is returned nullptr, maybe syntax error or not implemented element");
            return nullptr;
        }

        left = createExpression(tree, left, right, character);
    }   

    // If we are actually leave from cycle and if isSubExpression is true, 
    // it means we are not found end brecket symbol, so it's a invalid expression   
    if (isSubExpression)
    {
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
    //parse(tree, "((1 + 4) * 2) + 3 + 4");
    //parse(tree, "((1+4)*2)+3+4");
}

static std::int32_t testExprAdd = 1;

static void drawAddFunction(kubvc::algorithm::ASTree& tree, char* buf, const std::size_t& size, const std::int32_t& id)
{
    auto idStr = std::to_string(id);
    
    ImGui::Text(idStr.c_str());
    
    ImGui::SameLine();
    if (ImGui::InputText(("##" + idStr + "_ExprInputText").c_str(), buf, size))
    {
        parse(tree, buf);
    }
    
    ImGui::SameLine();
    
    ImGui::PushID(("##" + idStr + "_ExprButton").c_str());
    if (ImGui::Button("-"))
    {
        // TODO: Remove current expression
        testExprAdd--;
    }

    ImGui::PopID();
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

    kubvc::algorithm::ASTree tree;
    createTree(tree);    

    constexpr auto MAX_BUFFER_SIZE = 1024;
    char buf[MAX_BUFFER_SIZE] = { '\0' };

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
                if (ImGui::Button("+"))
                {
                    testExprAdd++;
                }

                ImGui::Separator();

                for (std::int32_t i = 0; i < testExprAdd; i++)
                {
                    drawAddFunction(tree, buf, IM_ARRAYSIZE(buf), i);
                }
               

                ImGui::Separator();

                ImGui::Text("AST:");   
                static bool listStyleTree = false;
                ImGui::Checkbox("List style for tree", &listStyleTree);
                if (listStyleTree)
                {
                    showTreeList(tree.getRoot());
                }
                else
                {
                    showTreeVisual(tree);
                }
            }
            ImGui::End();

            if (ImGui::Begin("Viewer"))
            {           
                auto size = ImGui::GetContentRegionAvail();
                if (ImPlot::BeginPlot("PlotTitle", size)) 
                {	
                    static constexpr int test_data[] = {1,2,3,4,5};

                    ImPlot::PlotLine("PlotName", test_data, test_data, 4);

                    ImPlot::EndPlot();
                }
            }
            
            ImGui::End();       
        }
        

        gui->endDockspace();
        gui->end();
        window->swapAndPool();
    }

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
