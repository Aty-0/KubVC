#pragma once 
#include "ast_nodes.h"

#include "alg_helpers.h"
#include "operators.h"
#include "logger.h"

#include <glm/glm.hpp>

namespace kubvc::algorithm {
    inline void NodeTraits<NodeTypes::Root>::calculate(double x, double y, double& result) {
        if (!child) {
            return;
        }
            
        child->calculate(x, y, result);            
    }

    inline void NodeTraits<NodeTypes::Variable>::calculate(double x, double y, double& result) {
        result = isParameter ? parameter : (m_value == 'y' ? y : x);
    }  

    inline void NodeTraits<NodeTypes::Function>::calculate(double x, double y, double& result) {
        KUB_ASSERT(argument != nullptr, "Argument is null in FunctionNode");
        
        double argumentResult = 0.0;
        argument->calculate(x, y, argumentResult);
        result = Helpers::computeFunction(name, argumentResult); 
    }
        
    inline void NodeTraits<NodeTypes::UnaryOperator>::calculate(double x, double y, double& result) {
        const bool isChildInvalid = child->getType() == NodeTypes::Invalid;
        if (child == nullptr || isChildInvalid)
            return;

        child->calculate(x, y, result);
        
        const auto type = getOperatorTypeByChar(operation);        
        if (type == Operators::Minus) {
            result = -result;
        }
    }
    
    inline void NodeTraits<NodeTypes::Operator>::calculate(double x, double y, double& result) {
        if (right == nullptr || left == nullptr) {
            return;
        }
        
        const bool isRightNodeInvalid = right->getType() == NodeTypes::Invalid;
        const bool isLeftNodeInvalid = left->getType() == NodeTypes::Invalid; 
        
        if (isRightNodeInvalid || isLeftNodeInvalid) {
            return;
        }

        double leftResult, rightResult = 0.0;
        left->calculate(x, y, leftResult);
        right->calculate(x, y, rightResult);
        const auto op = getOperatorTypeByChar(operation);
        switch(op) {
            case Operators::Equal: {
                result = rightResult;
                break;
            }
            case Operators::Plus: {
                result = leftResult + rightResult;
                break;
            }
            case Operators::Minus: {
                result = leftResult - rightResult;
                break;
            }
            case Operators::Multiplication: {
                result = leftResult * rightResult;
                break;
            }
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (glm::abs(rightResult) < std::numeric_limits<double>::min()) {  
                    result = std::numeric_limits<double>::quiet_NaN();
                    break;
                }
                
                result = leftResult / rightResult;
                break;
            }
            case Operators::Module: {
                result = glm::mod(leftResult, rightResult);
                break;
            }                        
            case Operators::Power: {
                result = glm::pow(leftResult, rightResult);
                break;
            }
            default:
                KUB_ASSERT(false, "Unknown type operator");
                break;
        }              
    }
        
    inline std::complex<double> NodeTraits<NodeTypes::Root>::calculateComplex(double x, double y) { 
        if (!child) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
        }
        
        return child->calculateComplex(x, y);
    }

    inline std::complex<double> NodeTraits<NodeTypes::Number>::calculateComplex([[maybe_unused]] double x, [[maybe_unused]] double y) { 
        return { m_value, 0.0 };
    }

    inline std::complex<double> NodeTraits<NodeTypes::ComplexNumber>::calculateComplex([[maybe_unused]] double x, [[maybe_unused]] double y) { 
        return m_value;
    }

    inline std::complex<double> NodeTraits<NodeTypes::Invalid>::calculateComplex([[maybe_unused]] double x, [[maybe_unused]] double y) { 
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
    }

    inline std::complex<double> NodeTraits<NodeTypes::Variable>::calculateComplex(double x, double y) { 
        if (isParameter)
            return { parameter, 0.0 };
            
        switch (m_value) {
            case 'z':
                return { x, y };
            case 'x':
                return { x, 0.0 };
            case 'y':
                return { y, 0.0 };
            case 'w':
                return { 0.0, 0.0 }; // TODO:         
        } 

        return { 0.0, 0.0 };
    }

    inline std::complex<double> NodeTraits<NodeTypes::UnaryOperator>::calculateComplex(double x, double y) { 
        const auto isChildInvalid = child->getType() == NodeTypes::Invalid;

        if (child == nullptr || isChildInvalid) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
        }
        
        const auto result = child->calculateComplex(x, y);
        const auto type = getOperatorTypeByChar(operation);        
        if (type == Operators::Minus) {
            return -result;
        }

        return result;
    }

    inline std::complex<double> NodeTraits<NodeTypes::Operator>::calculateComplex(double x, double y) { 
        if (right == nullptr || left == nullptr) {
            return std::complex<double>(std::numeric_limits<double>::quiet_NaN(), 
                std::numeric_limits<double>::quiet_NaN()); 
        }
        
        const auto isRightNodeInvalid = right->getType() == NodeTypes::Invalid;
        const auto isLeftNodeInvalid = left->getType() == NodeTypes::Invalid; 
        
        if (isRightNodeInvalid || isLeftNodeInvalid) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
        }

        const auto leftResult = left->calculateComplex(x, y);
        const auto rightResult = right->calculateComplex(x, y);
        const auto op = getOperatorTypeByChar(operation);
        switch(op) {
            case Operators::Equal: {
                return rightResult;
            }
            case Operators::Plus: {
                return leftResult + rightResult;
            }
            case Operators::Minus: {
                return leftResult - rightResult;
            }
            case Operators::Multiplication: {
                return leftResult * rightResult;
            }
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (std::abs(rightResult) < std::numeric_limits<double>::min()) {  
                    return std::complex<double>(std::numeric_limits<double>::quiet_NaN(), 
                        std::numeric_limits<double>::quiet_NaN()); 
                }
                
                return leftResult / rightResult;
            }
            case Operators::Module: {
                return {0.0, 0.0}; // TODO: 
            }                        
            case Operators::Power: {
                return std::pow(leftResult, rightResult);
            }
            default:
                KUB_ASSERT(false, "Unknown type operator");
                break;
        }  

        return { 0.0, 0.0 };
    }
 
    inline std::complex<double> NodeTraits<NodeTypes::Function>::calculateComplex(double x, double y) { 
        KUB_ASSERT(argument != nullptr, "Argument is null in FunctionNode");
        const auto argumentResult = argument->calculateComplex(x, y);
        
        // TODO: Actually I need to find what's type of variable we are using
        // If it's Z or W we are invoke something like a computeComplexFunction
        // If not invoke default computeFunction ?

        return Helpers::computeFunction(name, argumentResult.real()); 
    }


}