#pragma once
#include <string>
#include <cmath>
#include <algorithm>

#include "math_base.h"

namespace kubvc::algorithm
{
    class Helpers
    {
        public:
            using uchar = unsigned char;

            [[nodiscard]] static inline void toLowerCase(std::string& text)
            {
                std::transform(text.begin(), text.end(), text.begin(), std::tolower);
            }

            [[nodiscard]] static inline double computeFunction(const std::string& name, double x)
            { 
                auto func = math::containers::Functions.get(name);
                if (func == nullptr)
                {
                    return 0;
                }

                return func(x);
            }

            [[nodiscard]] static inline bool isLetter(uchar chr)
            {
                return std::isalpha(chr);
            }

            [[nodiscard]] static inline bool isNumber(const std::string& text)
            {
                return !text.empty() && std::find_if(text.begin(), text.end(), 
                [](auto c) 
                {
                    return std::isdigit(c) || isDot(c);
                }) != text.end();
            }

            [[nodiscard]] static inline bool isDigit(uchar chr)
            {
                return std::isdigit(chr);
            }
            
            [[nodiscard]] static inline bool isDot(uchar chr)
            {
                return chr == '.';
            }

            [[nodiscard]] static inline bool isUnaryOperator(uchar chr)
            {
                switch (chr)
                {
                    case '+':
                    case '-':
                        return true;
                }
                return false;
            }

            [[nodiscard]] static inline bool isOperator(uchar chr)
            {
                switch (chr)
                {
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '=':
                    case '^':
                        return true;
                }
                return false;
            }
    
            // TODO: Support for other brackets 
            [[nodiscard]] static inline bool isBracketStart(uchar chr)
            {
                return chr == '(';
            }

            [[nodiscard]] static inline bool isBracketEnd(uchar chr)
            {
                return chr == ')';
            }

            [[nodiscard]] static inline bool isEqualsSign(uchar chr)
            {
                return chr == '=';
            }

            [[nodiscard]] static inline bool isWhiteSpace(uchar chr)
            {
                return std::isspace(chr);
            }
    };
}