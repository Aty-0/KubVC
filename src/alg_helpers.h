#pragma once
#include <string>
#include <cmath>
#include <algorithm>
#include <array>

#include "math_base.h"

namespace kubvc::algorithm {
    class Helpers {
        public:
            using uchar = unsigned char;

            static inline double computeFunction(std::string_view name, double x) { 
                auto result = utility::container::get(math::containers::Functions, name);
                if (!result.has_value()) {
                    return 0;
                }

                return result.value()(x);
            }

            static inline constexpr uchar toLower(uchar chr) {
                return (chr >= 'A' && chr <= 'Z') ? (chr - 'A' + 'a') : chr;
            }

            static inline constexpr uchar toUpper(uchar chr) {
                return (chr >= 'a' && chr <= 'z') ? (chr - 'a' + 'A') : chr;
            }

            static inline constexpr std::string toLowerCase(std::string_view text) {
                std::string result;
                result.reserve(text.size());
                for (auto c : text) {
                    result.push_back(toLower(static_cast<uchar>(c)));
                }
                return result;
            }
       
            static inline constexpr std::string toUpperCase(std::string_view text) {
                std::string result;
                result.reserve(text.size());
                for (auto c : text) {
                    result.push_back(toUpper(static_cast<uchar>(c)));
                }
                return result;
            }

            static inline constexpr bool isLetter(uchar chr)  {
                return (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z');
            }

            static inline constexpr bool isDot(uchar chr) {
                return chr == '.';
            }

            static inline constexpr bool isDigit(uchar chr)  {
                return chr >= '0' && chr <= '9';
            }

            static inline constexpr bool isNumber(std::string_view text)  {
                return !text.empty() && std::all_of(text.begin(), text.end(), [](uchar c) {
                    return isDigit(c) || isDot(c);
                });
            }
            
            static inline constexpr bool isUnaryOperator(uchar chr) {
                return chr == '+' || chr == '-';
            }

            static inline constexpr bool isOperator(uchar chr) {
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

            static inline constexpr bool isBracketStart(uchar chr) {
                return chr == '(';
            }

            static inline constexpr bool isBracketEnd(uchar chr) {
                return chr == ')';
            }

            static inline constexpr bool isComma(uchar chr) {
                return chr == ',';
            }

            static inline constexpr bool isEqualsSign(uchar chr) {
                return chr == '=';
            }

            static inline constexpr bool isWhiteSpace(uchar chr) {
                constexpr std::array<uchar, 6> cases = { ' ', '\f', '\n', '\r', '\t', '\v' };
                return std::any_of(cases.begin(), cases.end(), [chr](uchar c) { 
                    return chr == c;
                });
            }
    };
}