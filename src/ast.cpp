#include "ast.h"

namespace kubvc::algorithm
{
    Node::~Node()
    {
        //DEBUG("Node destructor");
    }
    
    ASTree::ASTree() 
    {

    }   

    bool ASTree::isValidFrom(std::shared_ptr<kubvc::algorithm::Node> start) const 
    {
        // We are reached the end of tree 
        if (start == nullptr)
        {
            return false;
        }

        auto type = start->getType();
        switch (type)
        {
            case kubvc::algorithm::NodeTypes::Root:
            {
                auto node = static_cast<kubvc::algorithm::RootNode*>(start.get());
                return isValidFrom(node->child);
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
            {
                return true;            
            }
            case kubvc::algorithm::NodeTypes::Operator:
            {
                auto node = static_cast<kubvc::algorithm::OperatorNode*>(start.get());         
                bool resultLeft = true;
                bool resultRight = true;

                if (node->left != nullptr)
                {
                    resultLeft = isValidFrom(node->left);
                }

                if (node->right != nullptr)
                {
                    resultRight = isValidFrom(node->right);
                }

                return resultLeft && resultRight;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator:
            {
                auto node = static_cast<kubvc::algorithm::UnaryOperatorNode*>(start.get());         
                return isValidFrom(node->child);     
            }
            case kubvc::algorithm::NodeTypes::Function:
            {
                auto node = static_cast<kubvc::algorithm::FunctionNode*>(start.get());         
                if (node->argument != nullptr)
                {
                    return isValidFrom(node->argument);     
                }
                else 
                {
                    return false;
                }
            }
            case kubvc::algorithm::NodeTypes::Invalid:
            default:
                return false;
        }

        return false;
    }

    void ASTree::clear()
    {
        m_root.reset();
    }

    void ASTree::createRoot()
    {
        if (m_root != nullptr)
        {
            WARN("Root is already exist!");
            return;
        }

        m_root = createNode<RootNode>();
    }   

    std::shared_ptr<kubvc::algorithm::VariableNode> ASTree::createVariableNode(char value)
    {
        auto varNode = createNode<kubvc::algorithm::VariableNode>();
        varNode->value = value;    
        return varNode;
    }

    std::shared_ptr<kubvc::algorithm::NumberNode> ASTree::createNumberNode(double value)
    {
        auto numNode = createNode<kubvc::algorithm::NumberNode>();
        numNode->value = value;    
        return numNode;
    }

    std::shared_ptr<kubvc::algorithm::OperatorNode> ASTree::createOperatorNode(std::shared_ptr<kubvc::algorithm::Node> x, 
        std::shared_ptr<kubvc::algorithm::Node> y, char op)
    {
        auto opNode = createNode<kubvc::algorithm::OperatorNode>();
        opNode->operation = op;
        opNode->left = std::move(x);
        opNode->right = std::move(y);
        return opNode;
    }

    std::shared_ptr<kubvc::algorithm::UnaryOperatorNode> ASTree::createUnaryOperatorNode(std::shared_ptr<kubvc::algorithm::Node> x, char op)
    {
        auto opNode = createNode<kubvc::algorithm::UnaryOperatorNode>();
        opNode->operation = op;
        opNode->child = std::move(x);
        return opNode;
    }


    std::shared_ptr<kubvc::algorithm::InvalidNode> ASTree::createInvalidNode(const std::string& name)
    {
        auto invalidNode = createNode<kubvc::algorithm::InvalidNode>();
        invalidNode->name = name;
        return invalidNode;
    }

    std::shared_ptr<kubvc::algorithm::FunctionNode> ASTree::createFunctionNode(const std::string& name)
    {
        auto funcNode = createNode<kubvc::algorithm::FunctionNode>();
        funcNode->name = name;

        return funcNode;
    }
}