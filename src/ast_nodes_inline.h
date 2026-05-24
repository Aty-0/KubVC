#pragma once 
#include "ast_nodes.h"

#include "alg_helpers.h"
#include "operators.h"
#include "logger.h"

#include <glm/glm.hpp>

namespace kubvc::algorithm {
    inline double NodeTraits<NodeTypes::Invalid>::calculate([[maybe_unused]] double x, [[maybe_unused]] double y) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    inline double NodeTraits<NodeTypes::ComplexNumber>::calculate([[maybe_unused]] double x, [[maybe_unused]] double y) {
        // Complex numbers can't be used in real mode calculation
        return std::numeric_limits<double>::quiet_NaN();
    }

    inline double NodeTraits<NodeTypes::Number>::calculate([[maybe_unused]] double x, [[maybe_unused]] double y) {
        return m_value;
    }

    inline double NodeTraits<NodeTypes::Root>::calculate(double x, [[maybe_unused]] double y) {
        // Return x, because ast will be push in x value result from child node 
        return x; 
    }
    
    inline double NodeTraits<NodeTypes::Variable>::calculate(double x, double y) {
        return isParameter ? parameter : (m_value == 'y' ? y : x);
    }  

    inline double NodeTraits<NodeTypes::Function>::calculate(double x, [[maybe_unused]] double y) {
        // Same logic as root, ast will be push in x value result from child node 
        return Helpers::computeFunction(name, x); 
    }
        
    inline double NodeTraits<NodeTypes::UnaryOperator>::calculate(double x, [[maybe_unused]] double y) {        
        const auto type = getOperatorTypeByChar(operation);        
        return type == Operators::Minus ? -x : x;
    }

    inline double NodeTraits<NodeTypes::Operator>::calculate(double x, double y) {    
        // Same logic as root or unary, ast will be push in x, y values result from children nodes 
        const auto operatorType = getOperatorTypeByChar(operation);
        switch(operatorType) {
            case Operators::Equal: {
                return y;
            }
            case Operators::Plus: {
                return x + y;
            }
            case Operators::Minus: {
                return x - y;
            }
            case Operators::Multiplication: {
                return x * y;
            }
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (glm::abs(y) < std::numeric_limits<double>::min()) {  
                    return std::numeric_limits<double>::quiet_NaN();
                }
                
                return x / y;
            }
            case Operators::Module: {
                return glm::mod(x, y);
            }                        
            case Operators::Power: {
                return glm::pow(x, y);
            }
            default:
                KUB_ERROR("Unknown type operator: {}", std::string(1, operation));
                break;
        }     

        return std::numeric_limits<double>::quiet_NaN();         
    }
        
    inline std::complex<double> NodeTraits<NodeTypes::Root>::calculateComplex(double re, double im) { 
        return { re, im }; // Return complex number by args calculated in ast 
    }

    inline std::complex<double> NodeTraits<NodeTypes::Number>::calculateComplex([[maybe_unused]] double re, [[maybe_unused]] double im) { 
        return { m_value, 0.0 };
    }

    inline std::complex<double> NodeTraits<NodeTypes::ComplexNumber>::calculateComplex([[maybe_unused]] double x, [[maybe_unused]] double im) { 
        return m_value;
    }

    inline std::complex<double> NodeTraits<NodeTypes::Invalid>::calculateComplex([[maybe_unused]] double re, [[maybe_unused]] double im) { 
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
    }

    inline std::complex<double> NodeTraits<NodeTypes::Variable>::calculateComplex(double re, double im) { 
        if (isParameter)
            return { parameter, 0.0 };
            
        switch (m_value) {
            case 'z':
                return { re, im };
            case 'x':
                return { re, 0.0 };
            case 'y':
                return { re, 0.0 };
            case 'w':
                return { 0.0, 0.0 }; // TODO:         
        } 

        return { 0.0, 0.0 };
    }

    inline std::complex<double> NodeTraits<NodeTypes::UnaryOperator>::calculateComplex(double re, double im) { 
        const auto type = getOperatorTypeByChar(operation);
        const auto result = std::complex<double> { re, im };         
        return type == Operators::Minus ? -result : result;
    }
    
    inline std::complex<double> NodeTraits<NodeTypes::Operator>::calculateComplex([[maybe_unused]] double re, [[maybe_unused]] double im) {
        KUB_ASSERT(false, "Don't use calculateComplex for operator, use calculateComplexOperator instead!");
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
    }

    inline std::complex<double> NodeTraits<NodeTypes::Operator>::calculateComplexOperator(std::complex<double> left, std::complex<double> right) { 
        const auto operatorType = getOperatorTypeByChar(operation);
        switch(operatorType) {
            case Operators::Equal: {
                return right;
            }
            case Operators::Plus: {
                return left + right;
            }
            case Operators::Minus: {
                return left - right;
            }
            case Operators::Multiplication: {
                return left * right;
            }
            case Operators::Division: {                
                // If we are too close to zero we are set result as NaN
                if (std::abs(right) < std::numeric_limits<double>::min()) {  
                    return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; 
                }
                
                return left / right;
            }
            case Operators::Module: {
                return {0.0, 0.0}; // TODO: 
            }                        
            case Operators::Power: {
                return std::pow(left, right);
            }
            default: {
                //KUB_ASSERT(false, "Unknown type operator: {}", std::string(1, operation));
                KUB_ERROR("Unknown type operator: {}", std::string(1, operation));
                break;
            }
        }  

        return { 0.0, 0.0 };
    }
 
    inline std::complex<double> NodeTraits<NodeTypes::Function>::calculateComplex(double re, double im) { 
        KUB_ASSERT(argument != nullptr, "Argument is null in FunctionNode");
        const auto argumentResult = std::complex<double> { re, im };
        
        // TODO: Actually I need to find what's type of variable we are using
        // If it's Z or W we are invoke something like a computeComplexFunction
        // If not invoke default computeFunction ?

        return Helpers::computeFunction(name, argumentResult.real()); 
    }


}