#pragma once
#include "singleton.h"
#include "nodeTypes.h"
#include "alg_helpers.h"
#include "logger.h"

#include "container.h"

// TODO:
//#include "function_handler.h"
#include "application_config.h"
#include <functional>

#include <string>
#include <algorithm>
#include <optional>
#include <stack>

#ifdef KUB_ENABLE_LEXER_DEBUG_LOG
    #define KUB_LEXER_DEBUG(fmt, ...) KUB_DEBUG(fmt, ##__VA_ARGS__)
#else 
    #define KUB_LEXER_DEBUG(fmt, ...)
#endif

namespace kubvc::algorithm {
    struct Token {
        enum class Types {
            None,
            Number, 
            ComplexNumber,
            Variable,
            Function,
            Operator, 
            UnaryOperator, 
            Comma, 
            BracketStart,
            BracketEnd
        };

        Types type;
        std::string value;
    }; 
    
    class Lexer : public utility::Singleton<Lexer> {
        public:
            [[nodiscard]] std::optional<std::vector<Token>> tokenize(std::string_view str, bool useShuntingYard = true, std::size_t startFromPos = 0);
            [[nodiscard]] std::string getLastError() const { return m_lastErrorMessage; }
            
            void print(const std::vector<Token>& tokens);

        private:
            // Parse text while predicate is stay true
            [[nodiscard]] std::optional<std::string> parseWhile(std::string_view str, std::function<bool(algorithm::Helpers::uchar)> predicate);
            // Return text in brackets 
            [[nodiscard]] std::optional<std::string> parseTextInBrackets(std::string_view str);
            [[nodiscard]] std::vector<Token> shuntingYardAlgorithm(const std::vector<Token>& in);
            
            [[nodiscard]] constexpr algorithm::Helpers::uchar peek(const std::size_t pos, std::string_view str);
            
            template<typename... Args>
            void saveLastError(std::format_string<Args...> fmt, Args&&... args);

            std::string m_lastErrorMessage;
    };


    template<typename... Args>
    inline void Lexer::saveLastError(std::format_string<Args...> fmt, Args&&... args) {
        const auto formatedString = std::format(fmt, std::forward<Args>(args)...);
        KUB_ERROR("[lexer] {}", formatedString);
        m_lastErrorMessage = formatedString;
    }


    inline std::vector<Token> Lexer::shuntingYardAlgorithm(const std::vector<Token>& in) {
        if (in.size() == 0) {
            KUB_FATAL("shuntingYardAlgorithm: size is zero");
            return in;
        }
        
        static constexpr std::initializer_list<std::pair<char, std::uint8_t>> operatorPriority = {
            { '+', 0 },
            { '-', 0 },
            { '*', 1 },
            { '/', 1 },
            { '^', 2 },
        };

        std::vector<Token> output;
        std::stack<Token> stack;
        for (const auto& token : in) {
            switch (token.type) {                    
                case Token::Types::Variable:
                case Token::Types::ComplexNumber:
                case Token::Types::Number: {
                    output.push_back(token);

                    if (!stack.empty()) {
                        const auto top = stack.top();
                        if (top.type != Token::Types::UnaryOperator) {
                            break;  
                        }
                        output.push_back(top); 
                        stack.pop();                    
                    }
                    
                    break;
                }
                case Token::Types::Function: {
                    stack.push(token);
                    break;
                }
                case Token::Types::Comma: {
                      while (!stack.empty()) {
                        const auto top = stack.top();
                        if (top.type == Token::Types::BracketStart) {
                            break;
                        }

                        output.push_back(top);
                        stack.pop();
                    }
                    break;
                }
                case Token::Types::UnaryOperator: {
                    stack.push(token);         
                    break;                
                }
                case Token::Types::Operator: {
                        const auto currentOperator = token.value.at(0);
                        const auto currentOperatorPriority = utility::container::get(operatorPriority, currentOperator);
                        
                        while (!stack.empty()) {
                            const auto top = stack.top();                            
                            
                            if (top.type == Token::Types::Operator || top.type == Token::Types::UnaryOperator) {
                                const auto topOperatorPriority = utility::container::get(operatorPriority, currentOperator);
                                const auto operatorIsPower = currentOperator == '^';
                                
                                // First we are check on unary, because it has a highest priority and we don't need to pop it
                                // Second we are check on isOperatorIsPower because power is right associative, other operators is left associative
                                const bool shouldPop = top.type != Token::Types::UnaryOperator && 
                                    (operatorIsPower ? currentOperatorPriority < topOperatorPriority 
                                    : currentOperatorPriority <= topOperatorPriority);                                                                
                                if (shouldPop) {
                                    output.push_back(top);
                                    stack.pop();
                                } else {
                                    break;
                                }
                            } else {
                                break;
                            }
                        }

                        // Push op1 to stack
                        stack.push(token);
                        break;                
                    }
            
                case Token::Types::BracketStart: {
                    stack.push(token);
                    break;
                }
                case Token::Types::BracketEnd: {
                    while (!stack.empty()) {
                        const auto top = stack.top();
                        if (top.type == Token::Types::BracketStart) {                            
                            break;
                        }
                        output.push_back(top);
                        stack.pop();
                    }
                    
                    if (!stack.empty()) {
                        const auto top = stack.top();
                        if (top.type == Token::Types::BracketStart) {
                            stack.pop();
                        }
                    }
                    
                    if (!stack.empty()) {
                        const auto top = stack.top();
                        if (top.type == Token::Types::Function) {
                            output.push_back(top);
                            stack.pop();
                        }
                    }
                    
                    break;
                }

                default:
                    KUB_FATAL("Unknown token type");
                    break;
            }
        }

        while(!stack.empty()) {
            output.push_back(stack.top());
            stack.pop();
        }

        return output;
    }

    inline std::optional<std::string> Lexer::parseWhile(std::string_view str, std::function<bool(algorithm::Helpers::uchar)> predicate) {
        if (str.empty() || predicate == nullptr)
            return std::nullopt;

        std::string returnText { };
        for (const auto chr : str) {
            if (predicate(chr)) {
                returnText.push_back(chr);
            } else {
                break;
            }
        }

        if (returnText.empty())
            return std::nullopt;

        return returnText;
    }

    // TODO: Support for other brackets
    inline std::optional<std::string> Lexer::parseTextInBrackets(std::string_view str) {
        if (str.size() < 3 || // Because minimal example is something like that -> (x)
            !algorithm::Helpers::isBracketStart(str.at(0))) {
            return std::nullopt;
        }        
        // Find index of end bracket 
        std::size_t index = 0;
        std::int32_t bracketsCount = 0;

        for (const auto chr : str) {
            if (algorithm::Helpers::isBracketStart(chr)) {
                bracketsCount++;
            } else if (algorithm::Helpers::isBracketEnd(chr)) {
                bracketsCount--;
                if (bracketsCount == 0) {
                    return std::string { str.substr(0, index + 1) };
                }
            }
            index++;
        }
        
        return std::nullopt;
    }

    inline constexpr algorithm::Helpers::uchar Lexer::peek(const std::size_t pos, std::string_view str) {
        if (pos >= str.length()) {
            KUB_LEXER_DEBUG("peek failed: pos >= str.length()");
            return '\0';
        }

        return str[pos];
    }  

#ifdef KUB_LEXER_DEBUG_ENABLE_TOKEN_PRINT
    inline void Lexer::print(const std::vector<Token>& tokens) {
        std::stringstream stream;
        stream << tokens.size() << " ";               
        stream << "[";               
        for (const auto& token : tokens) {
            stream << token.value << ",";
        }
        stream << "]";        

        KUB_LEXER_DEBUG("{}", stream.str());
        stream.clear();
    }
#else 
    inline void Lexer::print([[maybe_unused]] const std::vector<Token>& tokens) { }
#endif

    inline std::optional<std::vector<Token>> Lexer::tokenize(std::string_view str, bool useShuntingYard, std::size_t startFromPos) {
        // reset error message 
        m_lastErrorMessage = "";

        if (str.empty()) {
            saveLastError("input string is empty: nothing to tokenize");
            return std::nullopt;
        }

        KUB_LEXER_DEBUG("[tokenize] try to tokenize: {}", str);
        std::vector<Token> tokens = { };
        bool isOperatorOpen = false;
        std::int32_t bracketLayer = 0;    
        std::size_t pos = startFromPos;
        while (pos < str.size()) {
            auto current = peek(pos, str);
            const auto currentCharStr = std::string(1, current);

            KUB_LEXER_DEBUG("[tokenize] current character is {} pos:{}", currentCharStr, pos);
            if (algorithm::Helpers::isWhiteSpace(current)) {
                KUB_LEXER_DEBUG("[tokenize] skip whitespace");
                pos++;
                continue;
            }

            if (algorithm::Helpers::isDigit(current)) {
                const auto result = parseWhile(str.substr(pos, str.size()), 
                    [](algorithm::Helpers::uchar chr) { return algorithm::Helpers::isDigit(chr) || algorithm::Helpers::isDot(chr); });
                isOperatorOpen = false;
                if (result.has_value()) {
                    const auto number = result.value();
                    pos += number.size(); 
                    const auto token = Token {
                        Token::Types::Number,
                        number,
                    };
                    tokens.push_back(token);
                    KUB_LEXER_DEBUG("[tokenize] parserd number is {}", number);
                } else {
                    saveLastError("failed to parse number: invalid numeric format at position {}", pos);
                    return std::nullopt;
                }
            } else if (algorithm::Helpers::isLetter(current)) {
                const auto result = parseWhile(str.substr(pos, str.size()), [](algorithm::Helpers::uchar chr) { return algorithm::Helpers::isLetter(chr) || algorithm::Helpers::isDigit(chr); });
                isOperatorOpen = false;
                if (result.has_value()) {
                    const auto word = result.value();
                    const auto wordSize = word.size();     
                    // First we are try to find constant from list                
                    const auto constResult = utility::container::get(math::containers::Constants, std::string_view { word.data(), wordSize });
                    if (constResult.has_value()) {
                        KUB_LEXER_DEBUG("[tokenize] it's a constant");
                        const auto token = Token {
                            Token::Types::Number,
                            std::to_string(constResult.value()),
                        };   
                        tokens.push_back(token);
                        pos += wordSize;
                        continue;                                                                                     
                    }
                    // Or it's possible variable or function 
                    if (wordSize == 1) {
                        static const auto appConfig = application::ApplicationConfig::getInstance();
                        if (word == "i" && appConfig->getMode() == application::MathMode::Complex) {
                            const auto token = Token {
                                Token::Types::ComplexNumber,
                                word,
                            };
                            tokens.push_back(token);
                        } else {
                            const auto token = Token {
                                Token::Types::Variable,
                                word,
                            };
                            tokens.push_back(token);
                        }
                        pos++;

                        KUB_LEXER_DEBUG("[tokenize] parserd variable is {}", word);
                    } else {
                        pos += wordSize;
                        current = peek(pos, str);
                        KUB_LEXER_DEBUG("[tokenize] maybe some keyword pos:{} word:{} current char:{}", pos, word, currentCharStr);
                        // Convert name to lower case to avoid mismatch 
                        const auto lowerCaseName = kubvc::algorithm::Helpers::toLowerCase(word);
                        // First try to find function by name
                        const auto findResult = utility::container::find(math::containers::Functions, std::string_view { lowerCaseName.data(), lowerCaseName.size() });
                        // Then if we are find bracket and it's function we are trying to parse it
                        const auto brecketIsOpened = algorithm::Helpers::isBracketStart(current);
                        if (findResult && brecketIsOpened) { 
                            KUB_LEXER_DEBUG("[tokenize] we are find function in list and bracket is open");
                            // TODO: Not sure about text parsing                             
                            const auto parseTextResult = parseTextInBrackets(str.substr(pos, str.size()));
                            if (parseTextResult.has_value()) {
                                const auto token = Token {
                                    Token::Types::Function,
                                    lowerCaseName,
                                };
                                // Add function token                                 
                                KUB_LEXER_DEBUG("[tokenize] parsed func is {}", token.value);
                                tokens.push_back(token);      
                            } else {
                                saveLastError("failed to parse function arguments in brackets: {}", word);
                                return std::nullopt;
                            }
                        } else {
                            if (!brecketIsOpened) {
                                saveLastError("expected '(' after function name '{}'", word);
                            } else {
                                saveLastError("unknown identifier '{}' (not a function, constant, or variable)", word);
                            }
                            return std::nullopt;
                        }  
                    }
                } else {
                    saveLastError("failed to parse letters");
                    return std::nullopt;
                }

            } else if (algorithm::Helpers::isComma(current)) {
                KUB_LEXER_DEBUG("[tokenize] is comma");
                // TODO: Protection of double comma -> ,,1,
                if (bracketLayer == 0) {
                    saveLastError("comma can only be used inside function argument list (outside brackets layer 0)");
                    return std::nullopt;
                }

                const auto token = Token {
                    Token::Types::Comma,
                    currentCharStr,
                };
                tokens.push_back(token);  
                pos++;                                     
            } else if (algorithm::Helpers::isOperator(current)) {
                KUB_LEXER_DEBUG("[tokenize] is operator");
                bool isUnary = false;
                const auto size = tokens.size(); 

                // Weird check on unary: 
                if (size == 0) {
                    isUnary = true;
                } else {
                    const auto prevToken = tokens.at(size - 1);
                    // Kinda bad
                    if (prevToken.type == Token::Types::Operator || 
                        prevToken.type == Token::Types::UnaryOperator ||
                        prevToken.type == Token::Types::BracketStart || 
                        prevToken.type == Token::Types::Comma ||
                        prevToken.type == Token::Types::Function) {
                        isUnary = true;
                    }
                }
                const bool currentCharIsUnary = (current == '-' || current == '+');
                if (isUnary && !currentCharIsUnary) {
                    saveLastError("operator '{}' cannot be used as unary operator in this context, position {}", currentCharStr, pos);
                    return std::nullopt;
                }

                KUB_LEXER_DEBUG("[tokenize] is unary {}", isUnary);

                const auto token = Token {
                    isUnary ? Token::Types::UnaryOperator : Token::Types::Operator,
                    currentCharStr,
                };
                

                tokens.push_back(token);  
                pos++;             
                isOperatorOpen = !isUnary;
            } else if (algorithm::Helpers::isBracketStart(current)) { 
                KUB_LEXER_DEBUG("[tokenize] is bracket start");
                isOperatorOpen = false;
                // Increment a bracket layer 
                bracketLayer++;

                const auto token = Token {
                    Token::Types::BracketStart,
                    currentCharStr,
                };
                tokens.push_back(token);  
                pos++;             
            } else if (algorithm::Helpers::isBracketEnd(current)) {
                KUB_LEXER_DEBUG("[tokenize] is bracket end");
                if (bracketLayer == 0) {
                    saveLastError("closing bracket ')' without matching opening bracket '('");
                    return std::nullopt;
                }

                const auto token = Token {
                    Token::Types::BracketEnd,
                    currentCharStr,
                };
                pos++;             
                tokens.push_back(token);  

                bracketLayer--;
            } else {
                saveLastError("unexpected character '{}' (not a digit, letter, operator, bracket, or comma)", currentCharStr);
                return std::nullopt;                                
            }
        }
        
        // If operator is not closed on parse end
        if (isOperatorOpen == true) {
            saveLastError("incomplete expression: operator has no right operand");
            return std::nullopt;                
        }

        // If when loop is ended but some brackets are not closed it's invalid case
        if (bracketLayer > 0) {
            saveLastError("unclosed bracket(s): bracket(s) still open at end of input");
            return std::nullopt;
        }

        // If output is empty 
        if (tokens.size() == 0) {
            saveLastError("tokenization produced no tokens: input contains no valid expressions");
            return std::nullopt;
        }

        KUB_LEXER_DEBUG("[tokenize] finished, see next line for result");
        print(tokens);    

        return useShuntingYard ? shuntingYardAlgorithm(tokens) : tokens;
    }
}