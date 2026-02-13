#pragma once
#include <string>
#include <cmath>
#include <algorithm>

#include "math_base.h"

namespace kubvc::algorithm {
    class Helpers {
        public:
            using uchar = unsigned char;

            static inline void toLowerCase(std::string& text) {
                std::transform(text.begin(), text.end(), text.begin(), 
                    [](uchar chr) { return std::tolower(chr); }
                );
            }

            static inline double computeFunction(std::string_view name, double x) { 
                auto result = math::containers::Functions.get(name);
                if (!result.has_value()) {
                    return 0;
                }

                return result.value()(x);
            }

            static inline bool isLetter(uchar chr)  {
                return std::isalpha(chr);
            }

            static inline bool isNumber(std::string_view text)  {
                return !text.empty() && std::all_of(text.begin(), text.end(), [](uchar c) {
                    return std::isdigit(c) || isDot(c);
                });
            }

            static inline bool isDigit(uchar chr)  {
                return std::isdigit(chr);
            }
            
            static inline bool isDot(uchar chr) {
                return chr == '.';
            }

            static inline bool isUnaryOperator(uchar chr) {
                switch (chr) {
                    case '+':
                    case '-':
                        return true;
                }
                return false;
            }

            static inline bool isOperator(uchar chr) {
                switch (chr) {
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '=':
                    case '^':
                    case '%':
                        return true;
                }
                return false;
            }
    
            // TODO: Support for other brackets 
            static inline bool isBracketStart(uchar chr) {
                return chr == '(';
            }

            static inline bool isBracketEnd(uchar chr) {
                return chr == ')';
            }

            static inline bool isEqualsSign(uchar chr) {
                return chr == '=';
            }

            static inline bool isWhiteSpace(uchar chr) {
                return std::isspace(chr);
            }
    };
}