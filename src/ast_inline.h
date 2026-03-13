#pragma once 
#include "ast.h"

namespace kubvc::algorithm { 
    inline void NodeTraits<NodeTypes::Root>::calculate(const double& n, double& result) {
        if (child == nullptr)
            return;
            
        child->calculate(n, result);            
    }

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

    inline void ASTree::clear()  {        
        if (isRootExist()) {
            clearFrom(m_root);
            m_root = nullptr;
        } else {
            KUB_DEBUG("Can't clear ast because root is nullptr");
        }
    }

    inline void ASTree::clearFrom(std::shared_ptr<kubvc::algorithm::INode> start) {
        if (start == nullptr) {
            return;
        }

        const auto type = start->getType();
        switch (type) {
            case kubvc::algorithm::NodeTypes::Root: {
                auto node = castToNodePtr<NodeTypes::Root>(start);
                clearFrom(node->child);
                node->child = nullptr;
                break;
            }
            case kubvc::algorithm::NodeTypes::Operator: {
                auto node = castToNodePtr<NodeTypes::Operator>(start);         
                
                // Recursively clear left and right subtrees
                if (node->left != nullptr) {
                    clearFrom(node->left);
                    node->left = nullptr;
                }

                if (node->right != nullptr) {
                    clearFrom(node->right);
                    node->right = nullptr;
                }
                break;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                auto node = castToNodePtr<NodeTypes::UnaryOperator>(start);         
                if (node->child != nullptr) {
                    clearFrom(node->child);
                    node->child = nullptr;
                }
                break;     
            }
            case kubvc::algorithm::NodeTypes::Function: {
                auto node = castToNodePtr<NodeTypes::Function>(start);         
                if (node->argument != nullptr) {
                    clearFrom(node->argument);
                    node->argument = nullptr;
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
            case kubvc::algorithm::NodeTypes::Invalid:
                // Nothing to clear 
                break;
            default:
                KUB_ASSERT(true, "Unknown type");
                break;
        }

    }
    
    inline bool ASTree::validate() const {
        if (!isRootExist()) {
            return false;
        } 

        return validateFrom(castToINodePtr<NodeTypes::Root>(m_root)); 
    }

    inline bool ASTree::validateFrom(std::shared_ptr<kubvc::algorithm::INode> start) const {
        // We are reached the end of tree 
        if (start == nullptr) {
            return false;
        }

        const auto type = start->getType();
        switch (type) {
            case kubvc::algorithm::NodeTypes::Root: {
                auto node = castToNodePtr<NodeTypes::Root>(start);
                return validateFrom(node->child);
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
                return true;            
            case kubvc::algorithm::NodeTypes::Operator: {
                auto node = castToNodePtr<NodeTypes::Operator>(start);         
                bool resultLeft = true;
                bool resultRight = true;

                if (node->left != nullptr) {
                    resultLeft = validateFrom(node->left);
                }

                if (node->right != nullptr) {
                    resultRight = validateFrom(node->right);
                }

                return resultLeft && resultRight;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                auto node = castToNodePtr<NodeTypes::UnaryOperator>(start);         
                return validateFrom(node->child);     
            }
            case kubvc::algorithm::NodeTypes::Function: {
                auto node = castToNodePtr<NodeTypes::Function>(start);         
                if (node->argument != nullptr) {
                    return validateFrom(node->argument);     
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
}