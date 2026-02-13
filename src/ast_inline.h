#pragma once 
#include "ast.h"

namespace kubvc::algorithm { 
    inline void NodeTraits<NodeTypes::Function>::calculate(const double& n, double& result) {
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
        
    inline void NodeTraits<NodeTypes::UnaryOperator>::calculate(const double& n, double& result) {
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
    
    inline void NodeTraits<NodeTypes::Operator>::calculate(const double& n, double& result) {
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

    inline NodePtr<NodeTypes::Variable> ASTree::createVariableNode(char value) {
        auto varNode = createNode<NodeTypes::Variable>();
        varNode->setValue(value);    
        return varNode;
    }

    inline NodePtr<NodeTypes::Number> ASTree::createNumberNode(double value) {
        auto numNode = createNode<NodeTypes::Number>();
        numNode->setValue(value);   
        return numNode;
    }

    inline NodePtr<NodeTypes::Operator> ASTree::createOperatorNode(std::shared_ptr<INode> x, 
        std::shared_ptr<INode> y, char op) {
        auto opNode = createNode<NodeTypes::Operator>();
        opNode->operation = op;
        opNode->left = std::move(x);
        opNode->right = std::move(y);
        return opNode;
    }

    inline NodePtr<NodeTypes::UnaryOperator> ASTree::createUnaryOperatorNode(std::shared_ptr<kubvc::algorithm::INode> x, char op) {
        auto opNode = createNode<NodeTypes::UnaryOperator>();
        opNode->operation = op;
        opNode->child = std::move(x);
        return opNode;
    }


    inline NodePtr<NodeTypes::Invalid> ASTree::createInvalidNode(std::string_view name) {
        auto invalidNode = createNode<NodeTypes::Invalid>();
        invalidNode->name = name;
        return invalidNode;
    }

    inline NodePtr<NodeTypes::Function> ASTree::createFunctionNode(std::string_view name) {
        auto funcNode = createNode<NodeTypes::Function>();
        funcNode->name = name;
        return funcNode;
    }

    inline void ASTree::clear()  {        
        m_root.reset();
    }

    inline void ASTree::createRoot()  {
        if (m_root != nullptr)
        {
            KUB_WARN("Root is already exist!");
            return;
        }

        m_root = createNode<NodeTypes::Root>();
    }   

    inline bool ASTree::isValidFrom(std::shared_ptr<kubvc::algorithm::INode> start) const {
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
    inline NodePtr<NodeType> ASTree::createNode() const {
        static std::uint32_t id = 0;
        const auto node = std::make_shared<NodeTraits<NodeType>>();
        node->setId(id);
        id++;
        return node;
    }
}