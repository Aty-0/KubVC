#pragma once
#include <string>
#include <cmath>
#include <algorithm>

#include "math_base.h"

namespace kubvc::algorithm {
    class Helpers {
        public:
            using uchar = unsigned char;

            static inline auto toLowerCase(std::string& text) -> void {
                std::transform(text.begin(), text.end(), text.begin(), 
                    [](uchar chr) { return std::tolower(chr); }
                );
            }

            [[nodiscard]] static inline auto computeFunction(std::string_view name, double x) -> double { 
                auto result = math::containers::Functions.get(name);
                if (!result.has_value()) {
                    return 0;
                }

                return result.value()(x);
            }

            [[nodiscard]] static inline auto isLetter(uchar chr) -> bool {
                return std::isalpha(chr);
            }

            [[nodiscard]] static inline auto isNumber(std::string_view text) -> bool {
                return !text.empty() && std::all_of(text.begin(), text.end(), [](uchar c) {
                    return std::isdigit(c) || isDot(c);
                });
            }

            [[nodiscard]] static inline auto isDigit(uchar chr) -> bool {
                return std::isdigit(chr);
            }
            
            [[nodiscard]] static inline auto isDot(uchar chr) -> bool{
                return chr == '.';
            }

            [[nodiscard]] static inline auto isUnaryOperator(uchar chr) -> bool{
                switch (chr) {
                    case '+':
                    case '-':
                        return true;
                }
                return false;
            }

            [[nodiscard]] static inline auto isOperator(uchar chr) -> bool {
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
            [[nodiscard]] static inline auto isBracketStart(uchar chr) -> bool {
                return chr == '(';
            }

            [[nodiscard]] static inline auto isBracketEnd(uchar chr) -> bool {
                return chr == ')';
            }

            [[nodiscard]] static inline auto isEqualsSign(uchar chr) -> bool {
                return chr == '=';
            }

            [[nodiscard]] static inline auto isWhiteSpace(uchar chr) -> bool {
                return std::isspace(chr);
            }
    };
}