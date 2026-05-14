#pragma once 
#include "alg_helpers.h"

namespace kubvc::algorithm {
    enum class Operators {
        Plus, 
        Minus,
        Multiplication,
        Division,
        Power,
        Equal,
        Module,
        Unknown,
    };

    [[nodiscard]] static inline Operators getOperatorTypeByChar(Helpers::uchar chr) {   
        switch (chr) {
            case '+':
                return Operators::Plus;
            case '-':
                return Operators::Minus;
            case '*':
                return Operators::Multiplication;
            case '/':
                return Operators::Division;
            case '=':
                return Operators::Equal;
            case '^':
                return Operators::Power;
            case '%':
                return Operators::Module;
        }
        return Operators::Unknown;
    } 
}