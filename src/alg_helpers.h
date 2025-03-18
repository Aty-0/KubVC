#pragma once
#include <string>

namespace kubvc::algorithm
{
    class AlgorithmHelpers
    {
        public:
            [[nodiscard]] static inline bool isLetter(char chr)
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

            [[nodiscard]] static inline bool isDigit(char chr)
            {
                return std::isdigit(chr);
            }
            
            [[nodiscard]] static inline bool isDot(char chr)
            {
                return chr == '.';
            }

            [[nodiscard]] static inline bool isOperator(char chr)
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
            [[nodiscard]] static inline bool isBracketStart(char chr)
            {
                return chr == '(';
            }

            [[nodiscard]] static inline bool isBracketEnd(char chr)
            {
                return chr == ')';
            }

            [[nodiscard]] static inline bool isEqualsSign(char chr)
            {
                return chr == '=';
            }

            [[nodiscard]] static inline bool isWhiteSpace(char chr)
            {
                return chr == ' ';
            }
    };
}