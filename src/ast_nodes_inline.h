#pragma once 
#include "ast_nodes.h"

#include "alg_helpers.h"
#include "operators.h"
#include "logger.h"

#include <glm/glm.hpp>

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
}