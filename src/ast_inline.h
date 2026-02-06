#pragma once 
#include "ast.h"

namespace kubvc::algorithm { 
    inline auto NodeTraits<NodeTypes::Function>::calculate(const double& n, double& result) -> void {
        if (argument == nullptr) {
            KUB_ERROR("[FunctionNode] Argument is null");
            return;
        }
        
        switch (argument->getType()) {
            case NodeTypes::Operator:
            case NodeTypes::Function:
            case NodeTypes::UnaryOperator:
            case NodeTypes::Number: {
                double argumentResult = 0.0;
                argument->calculate(n, argumentResult);
                result = Helpers::computeFunction(name, argumentResult); 
                break;
            }
            case NodeTypes::Variable:
                result = Helpers::computeFunction(name, n); 
                break;
        }
    }
        
    inline auto NodeTraits<NodeTypes::UnaryOperator>::calculate(const double& n, double& result) -> void {
        const bool isChildIsInvalid = child->getType() == NodeTypes::Invalid;
        if (child == nullptr || isChildIsInvalid)
            return;

        child->calculate(n, result);
        
        auto op = getOperatorFrom(operation);
        switch(op) {
            case Operators::Plus:
                result = std::fabs(result);
                break;
            case Operators::Minus:
                result = -result;
                break;
        }
    }
    
    inline auto NodeTraits<NodeTypes::Operator>::calculate(const double& n, double& result) -> void {
        const bool isRightNodeInvalid = right->getType() == NodeTypes::Invalid;
        const bool isLeftNodeInvalid = left->getType() == NodeTypes::Invalid; 
        if ((right == nullptr || left == nullptr) 
            || isRightNodeInvalid 
            || isLeftNodeInvalid) {
            return;
        }

        double firstResult, secondResult = 0.0;
        left->calculate(n, firstResult);
        right->calculate(n, secondResult);
        const auto op = getOperatorFrom(operation);
        switch(op) {
            case Operators::Equal:
                result = secondResult;
                break;
            case Operators::Plus:
                result = firstResult + secondResult;
                break;
            case Operators::Minus:
                result = firstResult - secondResult;
                break;
            case Operators::Multiplication:
                result = firstResult * secondResult;
                break;
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (glm::abs(secondResult) < std::numeric_limits<double>::min()) {  
                    result = std::numeric_limits<double>::quiet_NaN();
                    break;
                }
                
                result = firstResult / secondResult;
                break;
            }
            case Operators::Module:                        
                result = glm::mod(firstResult, secondResult);
                break;
            case Operators::Power:
                result = glm::pow(firstResult, secondResult);
                break;
        }              
    }

    inline auto ASTree::createVariableNode(char value) -> NodePtr<NodeTypes::Variable> {
        auto varNode = createNode<NodeTypes::Variable>();
        varNode->setValue(value);    
        return varNode;
    }

    inline auto ASTree::createNumberNode(double value) -> NodePtr<NodeTypes::Number> {
        auto numNode = createNode<NodeTypes::Number>();
        numNode->setValue(value);   
        return numNode;
    }

    inline auto ASTree::createOperatorNode(std::shared_ptr<INode> x, 
        std::shared_ptr<INode> y, char op) -> NodePtr<NodeTypes::Operator> {
        auto opNode = createNode<NodeTypes::Operator>();
        opNode->operation = op;
        opNode->left = std::move(x);
        opNode->right = std::move(y);
        return opNode;
    }

    inline auto ASTree::createUnaryOperatorNode(std::shared_ptr<kubvc::algorithm::INode> x, char op) -> NodePtr<NodeTypes::UnaryOperator> {
        auto opNode = createNode<NodeTypes::UnaryOperator>();
        opNode->operation = op;
        opNode->child = std::move(x);
        return opNode;
    }


    inline auto ASTree::createInvalidNode(std::string_view name) -> NodePtr<NodeTypes::Invalid> {
        auto invalidNode = createNode<NodeTypes::Invalid>();
        invalidNode->name = name;
        return invalidNode;
    }

    inline auto ASTree::createFunctionNode(std::string_view name) -> NodePtr<NodeTypes::Function> {
        auto funcNode = createNode<NodeTypes::Function>();
        funcNode->name = name;
        return funcNode;
    }

    inline auto ASTree::clear() -> void {        
        m_root.reset();
    }

    inline auto ASTree::createRoot() -> void {
        if (m_root != nullptr)
        {
            KUB_WARN("Root is already exist!");
            return;
        }

        m_root = createNode<NodeTypes::Root>();
    }   

    inline auto ASTree::isValidFrom(std::shared_ptr<kubvc::algorithm::INode> start) const -> bool {
        // We are reached the end of tree 
        if (start == nullptr) {
            return false;
        }

        auto type = start->getType();
        switch (type) {
            case kubvc::algorithm::NodeTypes::Root: {
                auto node = castToNodePtr<NodeTypes::Root>(start);
                return isValidFrom(node->child);
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
                return true;            
            case kubvc::algorithm::NodeTypes::Operator: {
                auto node = castToNodePtr<NodeTypes::Operator>(start);         
                bool resultLeft = true;
                bool resultRight = true;

                if (node->left != nullptr) {
                    resultLeft = isValidFrom(node->left);
                }

                if (node->right != nullptr) {
                    resultRight = isValidFrom(node->right);
                }

                return resultLeft && resultRight;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                auto node = castToNodePtr<NodeTypes::UnaryOperator>(start);         
                return isValidFrom(node->child);     
            }
            case kubvc::algorithm::NodeTypes::Function: {
                auto node = castToNodePtr<NodeTypes::Function>(start);         
                if (node->argument != nullptr) {
                    return isValidFrom(node->argument);     
                }
                else {
                    return false;
                }
            }
            case kubvc::algorithm::NodeTypes::Invalid:
            default:
                return false;
        }

        return false;
    }
    
    template <NodeTypes NodeType>
    inline auto ASTree::createNode() const -> NodePtr<NodeType> {
        static std::uint32_t id = 0;
        const auto node = std::make_shared<NodeTraits<NodeType>>();
        node->setId(id);
        id++;
        return node;
    }
}