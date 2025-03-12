#include "logger.h"
#include "renderer.h"
#include "window.h"
#include "ast.h"
#include "gui.h"
#include "alg_helpers.h"

static void showTree(std::shared_ptr<kubvc::algorithm::Node> start)
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
                showTree(node->child);
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
                showTree(node->left);            
                ImGui::Text("Right");
                showTree(node->right);
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
    opNode->left = x;
    opNode->right = y;
    return opNode;
}

static inline auto getCurrentChar(const std::size_t& cursor, const std::string& text) 
{
    if (cursor > text.size())
    {
        WARN("Cursor is out of bounds");
        return '\0';
    }
    
    return text[cursor];
}

static std::shared_ptr<kubvc::algorithm::Node> decideParser(const kubvc::algorithm::ASTree& tree, std::size_t& cursor, const std::string& text) 
{
    static auto parseNumbers = [](std::size_t& cursor, const std::string& text) {
        char character = getCurrentChar(cursor, text);
        std::string output = std::string();
        while(kubvc::algorithm::AlgorithmHelpers::isDigit(character) 
            || kubvc::algorithm::AlgorithmHelpers::isDot(character))
        {
            DEBUG("%s %c", output.c_str(), character);

            output += character;
            cursor++;
            character = getCurrentChar(cursor, text);
        }

        return output;
    };

    auto character = getCurrentChar(cursor, text);
    if (kubvc::algorithm::AlgorithmHelpers::isDigit(character))
    {
        DEBUG("Ho ho ho, we are want to parse a number...");
        auto out = parseNumbers(cursor, text);
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
    else if (kubvc::algorithm::AlgorithmHelpers::isOperator(character))
    {
        DEBUG("It's operator");
        cursor++;
        return createExpression(tree, nullptr, nullptr, character);
    }   
    else if (kubvc::algorithm::AlgorithmHelpers::isBracket(character))
    {
        // TODO:
    }   

    return nullptr;
};

static std::string getTypeName(const kubvc::algorithm::NodeTypes& type)
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

static void parse(const kubvc::algorithm::ASTree& tree, const std::string& text, const std::size_t cursor_pos = 0)
{
    std::size_t cursor = cursor_pos;
    auto root = static_cast<kubvc::algorithm::RootNode*>(tree.getRoot().get());
    // TODO: How we can parse a begining of expression like "y=..." 

    //// FIXME: too bad, it's static 
    //static std::shared_ptr<kubvc::algorithm::OperatorNode> prevOpNode = nullptr;
    //static std::shared_ptr<kubvc::algorithm::NumberNode> prevLeftNum = nullptr;
//
    //auto node = decideParser(tree, cursor, text);
    //if (node == nullptr || cursor > text.size())
    //{
    //    DEBUG("It's end or what ?");
    //    return;
    //}
//
    //DEBUG("Cursor: %i Current node: %s", cursor, getTypeName(node->getType()).c_str());
//
    //if (node->getType() == kubvc::algorithm::NodeTypes::Number)
    //{
    //    if (prevLeftNum == nullptr)
    //    {
    //        DEBUG("Saved shit");
    //        prevLeftNum = std::dynamic_pointer_cast<kubvc::algorithm::NumberNode>(node);
    //    }
    //}
//
    //if (node->getType() == kubvc::algorithm::NodeTypes::Operator)
    //{
    //    if (root->child == nullptr)
    //    {
    //        DEBUG("Set root child");
    //        root->child = node;
    //    }
//
    //    prevOpNode = std::dynamic_pointer_cast<kubvc::algorithm::OperatorNode>(node);            
    //}
    //
    //if (prevOpNode != nullptr)
    //{
    //    if (prevOpNode != std::dynamic_pointer_cast<kubvc::algorithm::OperatorNode>(node)) // FIXME: Possibly hard operation 
    //    {         
    //        if (prevLeftNum != nullptr)
    //        {
    //            prevOpNode->left = prevLeftNum;
    //            prevLeftNum = nullptr;
    //        }   
//
    //        if (prevOpNode->left != nullptr)
    //        {
    //            DEBUG("Set right node");
    //            prevOpNode->right = node;
    //        }
    //        else
    //        {
    //            DEBUG("Set left node");
    //            prevOpNode->left = node;           
    //        }
    //    }
    //}
    //
    //parse(tree, text, cursor);
//
}

static void testTree(kubvc::algorithm::ASTree& tree)
{ 
    tree.makeRoot();
    //auto root = static_cast<kubvc::algorithm::RootNode*>(tree.getRoot().get());
    //auto secondRoot = tree.createNode<kubvc::algorithm::RootNode>();
    //
    //secondRoot->child = createExpression(tree, createNumberNode(tree, 5), createNumberNode(tree, 3), '*');
    //root->child = createExpression(tree, createNumberNode(tree, 1), secondRoot, '+');
    parse(tree, "2.4346+2");
    //parse(tree, "2+2+2");
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

    // Run main loop 
    while (!window->shouldClose())
    {
        render->render();
        gui->begin();
        
        // TODO: Convert function into gui class  
        if (ImGui::Begin("Toolbox"))
        {
            ImGui::Text("TODO: Add functions");    

            ImGui::Separator();
            ImGui::Text("AST:");    
            showTree(tree.getRoot());
         
            ImGui::End();
        }

        if (ImGui::Begin("Viewer"))
        {
            gui->draw();
            ImGui::End();        
        }

        gui->end();
        window->swapAndPool();
    }

    // Destroy application components
    gui->destroy();
    window->destroy();

    return 0;
}
