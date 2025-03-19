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

static void showTreeLegacy(std::shared_ptr<kubvc::algorithm::Node> start)
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
            nodeName = "##" + std::to_string(node->id);
            if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
            {
                showTreeLegacy(node->child);
                ImGui::TreePop();  
            }  
            break;
        }
        case kubvc::algorithm::NodeTypes::Number:
        {
            auto node = static_cast<kubvc::algorithm::NumberNode*>(start.get());         
            nodeName = std::to_string(node->value) + "##" + std::to_string(node->id);

            if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
            {
                ImGui::TreePop();  
            }  
            break;            
        }
        case kubvc::algorithm::NodeTypes::Operator:
        {
            auto node = static_cast<kubvc::algorithm::OperatorNode*>(start.get());         
            nodeName = std::string(1, node->operation) +  "##" + std::to_string(node->id); 
            if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
            {
                ImGui::Text("Left");
                showTreeLegacy(node->left);            
                ImGui::Text("Right");
                showTreeLegacy(node->right);
                ImGui::TreePop();  
            }  
            break;    
        }
        case kubvc::algorithm::NodeTypes::Variable:
        {
            auto node = static_cast<kubvc::algorithm::VariableNode*>(start.get());         
            nodeName = node->value + "##" + std::to_string(node->id); 
            if (ImGui::TreeNodeEx(nodeName.c_str(), flag))
            {
                ImGui::TreePop();  
            }  
            break;
        }
        default:
            ERROR("Unknown type or not implemented");
            break;
    }
}

enum IndentMode 
{
    None,
    Right,
    Left
};

static constexpr auto IndentAugment = 60.0f; 

static void showTree(std::shared_ptr<kubvc::algorithm::Node> start, ImVec2 offset, IndentMode mode = IndentMode::None)
{
    static float indent = 0.0f;
    // We are reached the end of tree 
    if (start == nullptr)
    {
        return;
    }


    auto type = start->getType();
    
    if (type == kubvc::algorithm::NodeTypes::Operator)
        mode = IndentMode::None;

    switch (mode)
    {
        case IndentMode::Left:
            ImGui::SetCursorPos(ImVec2((indent / 2) + offset.x, indent + offset.y));
            indent += IndentAugment;
            break;
        case IndentMode::Right:
            ImGui::SetCursorPos(ImVec2((indent / 2) + offset.x + 100.0f, (indent - IndentAugment) + offset.y));
            break;        
        default:
            indent += IndentAugment;
            if (type == kubvc::algorithm::NodeTypes::Operator)
                ImGui::SetCursorPos(ImVec2(offset.x, indent + offset.y));
            else
                ImGui::SetCursorPos(ImVec2(offset.x, offset.y));
            break;
    }

    auto nodeName = std::string();
    switch (type)
    {
        case kubvc::algorithm::NodeTypes::Root:
        {
            auto node = static_cast<kubvc::algorithm::RootNode*>(start.get());
            nodeName = ". #" + std::to_string(node->id);
            ImGui::Text(nodeName.c_str());
            indent = 0.0f;
            showTree(node->child, ImVec2(offset.x, offset.y + IndentAugment));
            break;
        }
        case kubvc::algorithm::NodeTypes::Number:
        {
            auto node = static_cast<kubvc::algorithm::NumberNode*>(start.get());         
            nodeName = std::to_string(node->value) + " #" + std::to_string(node->id);
            ImGui::Text(nodeName.c_str());
            break;            
        }
        case kubvc::algorithm::NodeTypes::Operator:
        {
            auto node = static_cast<kubvc::algorithm::OperatorNode*>(start.get());         
            nodeName = std::string(1, node->operation) +  " #" + std::to_string(node->id);
            ImGui::Text(nodeName.c_str());

            showTree(node->left, offset, IndentMode::Left);                                    
            showTree(node->right, offset, IndentMode::Right);
            
            break;    
        }
        case kubvc::algorithm::NodeTypes::Variable:
        {
            auto node = static_cast<kubvc::algorithm::VariableNode*>(start.get());         
            nodeName = node->value + " #" + std::to_string(node->id); 
            ImGui::Text(nodeName.c_str());
            break;
        }
        case kubvc::algorithm::NodeTypes::Function:
        {
            auto node = static_cast<kubvc::algorithm::FunctionNode*>(start.get());         
            nodeName = node->name + " #" + std::to_string(node->id); 
            ImGui::Text(nodeName.c_str());
            break;
        }
        case kubvc::algorithm::NodeTypes::Invalid:
        {
            auto node = static_cast<kubvc::algorithm::InvalidNode*>(start.get());         
            nodeName = node->name + " #" + std::to_string(node->id); 
            ImGui::TextColored(ImVec4(255, 0, 0, 255), nodeName.c_str());
            break;
        }
        default:
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "UNK");
            ERROR("Unknown type or not implemented");
            break;
    }
}

static void drawTreeChild(const kubvc::algorithm::ASTree& tree)
{            
    static constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders; 
    static auto pos = ImVec2(0,0);
    if (ImGui::BeginChild("TreeChildWindow", ImVec2(0,0), childFlags, ImGuiWindowFlags_NoInputs))
    { 
        //auto root = std::dynamic_pointer_cast<kubvc::algorithm::RootNode>(tree.getRoot());        
        //ImGui::Text("root child element info | id: %d | type: %s", root->child->id, getTypeName(root->child->getType()).c_str());

        showTree(tree.getRoot(), pos);
    }   
    
    auto io = ImGui::GetIO();
    if (ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Right))
    {
        pos.x += io.MouseDelta.x;
        pos.y += io.MouseDelta.y;
    }
    ImGui::EndChild();
}

// TODO: Actually variables can be named with much bigger names instade one char
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
    // TODO: Args

    return funcNode;
}

static inline auto getCurrentChar(const std::size_t& cursor, const std::string& text) 
{
    if (cursor > text.size())
    {
        FATAL("Cursor is out of bounds");
        return '\0';
    }
    
    return text[cursor];
}

static auto parseNumbers(std::size_t& cursor, const std::string& text)
{
    char character = getCurrentChar(cursor, text);
    std::string output = std::string();
    while(kubvc::algorithm::AlgorithmHelpers::isDigit(character) 
        || kubvc::algorithm::AlgorithmHelpers::isDot(character))
    {
        //DEBUG("%s %c", output.c_str(), character);
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
    while(kubvc::algorithm::AlgorithmHelpers::isLetter(character))
    {
        output += character;
        cursor++;
        character = getCurrentChar(cursor, text);
    }
    return output;
}

static std::shared_ptr<kubvc::algorithm::Node> parseFunction(const kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, const std::string& text)
{
    std::size_t cursor = cursor_pos;
    char character = getCurrentChar(cursor, text);
    std::string funcName = std::string();
    while(kubvc::algorithm::AlgorithmHelpers::isLetter(character))
    {
        funcName += character;
        cursor++;
        character = getCurrentChar(cursor, text);        
    } 
    
    if (cursor >= text.size())
        return createInvalid(tree, text);
  
    character = getCurrentChar(cursor, text);        
  
    if (kubvc::algorithm::AlgorithmHelpers::isBracketStart(character))
    {
        while(!kubvc::algorithm::AlgorithmHelpers::isBracketEnd(character))
        {
            cursor++;
            if (cursor >= text.size())
            {
                return createInvalid(tree, text);
            }

            character = getCurrentChar(cursor, text);        
            // TODO: Add args
        } 
    }
    else 
    {
        return createInvalid(tree, text);
    }

    return createFunction(tree, funcName);
}

static std::shared_ptr<kubvc::algorithm::Node> parseExpression(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor);

static std::shared_ptr<kubvc::algorithm::Node> parseElement(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, char currentChar)
{
    DEBUG("try to find something usefull for %c", currentChar); 

    // Skip white space if we are find it  
    if (kubvc::algorithm::AlgorithmHelpers::isWhiteSpace(currentChar))
    {
        DEBUG("Ignore white space in parse element stage"); 
        cursor++;
        currentChar = getCurrentChar(cursor, text);
    } 

    if (kubvc::algorithm::AlgorithmHelpers::isDigit(currentChar))
    {
        auto out = parseNumbers(cursor, text);

        DEBUG("Is digit %s", out.c_str());
        if (out.empty())
        {
            ERROR("Parse number has a empty output, hmm. Ignore!");
        }
        else
        {
            if (kubvc::algorithm::AlgorithmHelpers::isNumber(out))
            {
                return createNumberNode(tree, std::atof(out.c_str()));                
            }
            else
            {
                ERROR("Output is actually not a number, so ignore him! We catch that bad guy: %s", out.c_str());
            }
        }
    }    
    else if (kubvc::algorithm::AlgorithmHelpers::isLetter(currentChar))
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
            return parseFunction(tree, cursor - outSize, text);
        }
        else 
        {
            // TODO: What we need to do with constants 
            return createVariableNode(tree, currentChar);
        }
    }
    else if (kubvc::algorithm::AlgorithmHelpers::isBracketStart(currentChar))
    {
        cursor++;
        DEBUG("Bracket start");
        auto node = parseExpression(tree, text, cursor);
        return node;
    }

    DEBUG("Nothing found");

    return createInvalid(tree, "INV_NODE");
}

static std::shared_ptr<kubvc::algorithm::Node> parseExpression(const kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor)
{
    DEBUG("parseExpression | cursor: %d", cursor);
    std::shared_ptr<kubvc::algorithm::Node> right = parseElement(tree, text, cursor, getCurrentChar(cursor, text));
    
    while (true)
    {
        auto character = getCurrentChar(cursor, text);  
        DEBUG("Current character in expression cycle: %c", character); 

        // Ignore white spaces
        if (kubvc::algorithm::AlgorithmHelpers::isWhiteSpace(character))
        {
            DEBUG("Ignore white space in expression parse"); 
            cursor++;
            continue;
        }  
        // We are want to continue cycle 
        else if (kubvc::algorithm::AlgorithmHelpers::isBracketEnd(character))
        {
            DEBUG("End of bracket");
            cursor++;
            continue;
        }
        // If current character is not operator we are leave from cycle 
        else if (!kubvc::algorithm::AlgorithmHelpers::isOperator(character))
        {
            DEBUG("Leave from cycle");
            break;
        }

        // Augment cursor position 
        cursor++;  
        // Try to find something 
        auto left = parseElement(tree, text, cursor, getCurrentChar(cursor, text));
        if (left == nullptr)
        {
            ERROR("parseElement is returned nullptr, maybe syntax error or not implemented element");
            return nullptr;
        }

        right = createExpression(tree, left, right, character);
    }   

    return right;
}

static void parse(const kubvc::algorithm::ASTree& tree, const std::string& text, const std::size_t cursor_pos = 0)
{
    std::size_t cursor = cursor_pos;
    auto root = static_cast<kubvc::algorithm::RootNode*>(tree.getRoot().get());
    root->child = parseExpression(tree, text, cursor);
}

static void testTree(kubvc::algorithm::ASTree& tree)
{ 
    tree.clear();
    tree.makeRoot();
    //parse(tree, "((1 + 4) * 2) + 3 + 4");
    //parse(tree, "((1+4)*2)+3+4");
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
    testTree(tree);    
    constexpr auto MAX_BUFFER_SIZE = 1024;
    char buf[MAX_BUFFER_SIZE] = { '\0' };
        
    // Run main loop 
    while (!window->shouldClose())
    {
        render->render();
        gui->begin();
        
        // TODO: Convert function into gui class  
        if (ImGui::Begin("Toolbox"))
        {
            ImGui::Text("TODO: Add functions");

            if (ImGui::InputText("Parse Text", buf, IM_ARRAYSIZE(buf)))
            {
                parse(tree, buf);
            }

            if (ImGui::Button("Parse"))
            {
                parse(tree, buf);
            }

            ImGui::Separator();
            ImGui::Text("AST:");   

            static bool useLegacyTree = false;
            ImGui::Checkbox("use legacy tree", &useLegacyTree);
            if (useLegacyTree)
            {
                showTreeLegacy(tree.getRoot());
            }
            else
            {
                drawTreeChild(tree);
            }
        }
        ImGui::End();

        if (ImGui::Begin("Viewer"))
        {
            gui->draw();
        }
        ImGui::End();        

        gui->end();
        window->swapAndPool();
    }

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
