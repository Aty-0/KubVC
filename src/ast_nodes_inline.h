#pragma once 
#include "ast_nodes.h"

#include "alg_helpers.h"
#include "operators.h"
#include "logger.h"

#include <glm/glm.hpp>

namespace kubvc::algorithm {
    inline void NodeTraits<NodeTypes::Variable>::calculate(const double x, const double y, double& result) {
        if (m_value == 'x') { // TODO: Use variable dep
            result = x;
        } else if (m_value == 'y') {
            result = y;
        } /*else {
            // TODO: Can be param
        }*/
    }   

    inline void NodeTraits<NodeTypes::Root>::calculate(const double x, const double y, double& result) {
        if (child == nullptr)
            return;

        child->calculate(x, y, result);            
    }

    inline void NodeTraits<NodeTypes::Function>::calculate(const double x, const double y, double& result) {
        if (argument == nullptr) {
            KUB_ERROR("[FunctionNode] Argument is null");
            return;
        }
        
        double argumentResult = 0.0;
        argument->calculate(x, y, argumentResult);
        result = Helpers::computeFunction(name, argumentResult); 
    }
        
    inline void NodeTraits<NodeTypes::UnaryOperator>::calculate(const double x, const double y, double& result) {
        const bool isChildInvalid = child->getType() == NodeTypes::Invalid;
        if (child == nullptr || isChildInvalid)
            return;

        child->calculate(x, y, result);
        
        const auto op = getOperatorFrom(operation);        
        if (op == Operators::Minus) {
            result = -result;
        }
    }
    
    inline void NodeTraits<NodeTypes::Operator>::calculate(const double x, const double y, double& result) {
        const bool isRightNodeInvalid = right->getType() == NodeTypes::Invalid;
        const bool isLeftNodeInvalid = left->getType() == NodeTypes::Invalid; 
        if ((right == nullptr || left == nullptr) 
            || isRightNodeInvalid 
            || isLeftNodeInvalid) {
            return;
        }

        double leftResult, rightResult = 0.0;
        left->calculate(x, y, leftResult);
        right->calculate(x, y, rightResult);
        const auto op = getOperatorFrom(operation);
        switch(op) {
            case Operators::Equal:
                result = rightResult;
                break;
            case Operators::Plus:
                result = leftResult + rightResult;
                break;
            case Operators::Minus:
                result = leftResult - rightResult;
                break;
            case Operators::Multiplication:
                result = leftResult * rightResult;
                break;
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (glm::abs(rightResult) < std::numeric_limits<double>::min()) {  
                    result = std::numeric_limits<double>::quiet_NaN();
                    break;
                }
                
                result = leftResult / rightResult;
                break;
            }
            case Operators::Module:                        
                result = glm::mod(leftResult, rightResult);
                break;
            case Operators::Power:
                result = glm::pow(leftResult, rightResult);
                break;
        }              
    }
}