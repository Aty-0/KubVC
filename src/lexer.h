#pragma once
#include "singleton.h"
#include "nodeTypes.h"
#include "alg_helpers.h"
#include "logger.h"

#include "container.h"

// TODO:
//#include "function_handler.h"
#include <functional>

#include <string>
#include <algorithm>
#include <optional>
#include <stack>

//#define KUB_LEXER_DEBUG_ENABLE_TOKEN_PRINT
//#define KUB_ENABLE_LEXER_DEBUG_LOG

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
            std::optional<std::vector<Token>> tokenize(std::string_view str, bool useShuntingYard = true, std::size_t startFromPos = 0);
            

            void print(const std::vector<Token>& tokens);

        private:
            // Parse text while predicate is stay true
            std::optional<std::string> parseWhile(std::string_view str, std::function<bool(algorithm::Helpers::uchar)> predicate);
            // Return text in brackets 
            std::optional<std::string> parseTextInBrackets(std::string_view str);
            std::vector<Token> shuntingYardAlgorithm(const std::vector<Token>& in);
            
            algorithm::Helpers::uchar peek(const std::size_t pos, std::string_view str);
    };

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
        for (const auto token : in) {
            switch (token.type) {                    
                case Token::Types::Variable:
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
                                const auto topOperator = top.value.at(0);
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

    inline algorithm::Helpers::uchar Lexer::peek(const std::size_t pos, std::string_view str) {
        if (pos >= str.length()) {
            return '\0';
        }
        return str.at(pos);
    }  

    inline void Lexer::print(const std::vector<Token>& tokens) {
#ifdef KUB_LEXER_DEBUG_ENABLE_TOKEN_PRINT
        std::stringstream stream;
        stream << tokens.size() << " ";               
        stream << "[";               
        for (const auto token : tokens) {
            stream << token.value << ",";
        }
        stream << "]";        

        KUB_LEXER_DEBUG("{}", stream.str());
        stream.clear();
#endif
    }

    inline std::optional<std::vector<Token>> Lexer::tokenize(std::string_view str, bool useShuntingYard, std::size_t startFromPos) {
        if (str.empty()) {
            KUB_LEXER_DEBUG("[tokenize] input string is empty");
            return std::nullopt;
        }

        KUB_LEXER_DEBUG("[tokenize] try to tokenize: {}", str);
        std::vector<Token> tokens = { };
        bool isOperatorOpen = false;
        std::int32_t isBracketOpen = 0;    
        std::size_t pos = startFromPos;
        while (pos < str.size()) {
            auto current = peek(pos, str);
            KUB_LEXER_DEBUG("[tokenize] current character is {} pos:{}", std::string(1, current), pos);
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
                    KUB_ERROR("[tokenize] failed to parse number");
                    return std::nullopt;
                }

            } else if (algorithm::Helpers::isLetter(current)) {
                const auto result = parseWhile(str.substr(pos, str.size()), [](algorithm::Helpers::uchar chr) { return algorithm::Helpers::isLetter(chr) || algorithm::Helpers::isDigit(chr); });
                isOperatorOpen = false;
                if (result.has_value()) {
                    const auto word = result.value();
                    const auto wordSize = word.size();
                    if (wordSize == 1) {
                        const auto token = Token {
                            Token::Types::Variable,
                            word,
                        };
                        tokens.push_back(token);
                        pos++;

                        KUB_LEXER_DEBUG("[tokenize] parserd variable is {}", word);
                    } else {
                        pos += wordSize;
                        current = peek(pos, str);
                        KUB_LEXER_DEBUG("[tokenize] maybe some keyword pos:{} word:{} current char:{}", pos, word, current);
                        // First try to find function by name
                        const auto findResult = utility::container::find(math::containers::Functions, std::string_view { word.data(), word.size() });
                        // Then if we are find bracket and it's function we are trying to parse it
                        if (findResult && algorithm::Helpers::isBracketStart(current)) { 
                            KUB_LEXER_DEBUG("[tokenize] we are find function in list and bracket is open");
                            // TODO: Not sure about text parsing                             
                            const auto result = parseTextInBrackets(str.substr(pos, str.size()));
                            if (result.has_value()) {
                                // Convert name to lower case to avoid mismatch 
                                std::string funcName { word };
                                kubvc::algorithm::Helpers::toLowerCase(funcName);

                                const auto token = Token {
                                    Token::Types::Function,
                                    funcName,
                                };
                                // Add function token                                 
                                KUB_LEXER_DEBUG("[tokenize] parsed func is {}", token.value);
                                tokens.push_back(token);      
                            } else {
                                KUB_ERROR("[tokenize] brackets parse failed");
                                return std::nullopt;
                            }
                        } else {
                            const auto constResult = utility::container::get(math::containers::Constants, std::string_view { word.data(), word.size() });
                            if (constResult.has_value()) {
                                KUB_LEXER_DEBUG("[tokenize] it's a constant");
                                const auto token = Token {
                                    Token::Types::Number,
                                    std::to_string(constResult.value()),
                                };   
                                tokens.push_back(token);                                                             
                            } else {
                                // TODO: Actually we can check on implicit multiplication here
                                KUB_ERROR("[tokenize] unknown keyword or brecket are not open {}", word);
                                return std::nullopt;
                            }
                        }     
                    }
                } else {
                    KUB_ERROR("[tokenize] failed to parse letters");
                    return std::nullopt;
                }

            } else if (algorithm::Helpers::isComma(current)) {
                KUB_LEXER_DEBUG("[tokenize] is comma");
                // TODO: Protection of double comma -> ,,1,
                if (isBracketOpen == 0) {
                    KUB_ERROR("[tokenize] trying to use comma not in function");
                    return std::nullopt;
                }

                const auto token = Token {
                    Token::Types::Comma,
                    std::string(1, current),
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

                if (!(isUnary && (current == '-' || current == '+'))) {
                    KUB_ERROR("[tokenize] operator is marked as unary, but this operator can't be unary");
                    return std::nullopt;
                }

                KUB_LEXER_DEBUG("[tokenize] is unary {}", isUnary);

                const auto token = Token {
                    isUnary ? Token::Types::UnaryOperator : Token::Types::Operator,
                    std::string(1, current),
                };
                

                tokens.push_back(token);  
                pos++;             
                isOperatorOpen = !isUnary;
            } else if (algorithm::Helpers::isBracketStart(current)) { 
                KUB_LEXER_DEBUG("[tokenize] is bracket start");
                isOperatorOpen = false;
                // Increment a bracket layer 
                isBracketOpen++;

                const auto token = Token {
                    Token::Types::BracketStart,
                    std::string(1, current),
                };
                tokens.push_back(token);  
                pos++;             
            } else if (algorithm::Helpers::isBracketEnd(current)) {
                KUB_LEXER_DEBUG("[tokenize] is bracket end");
                if (isBracketOpen == 0) {
                    KUB_ERROR("[tokenize] found end bracket but isBracketOpen = false");
                    return std::nullopt;
                }

                const auto token = Token {
                    Token::Types::BracketEnd,
                    std::string(1, current),
                };
                pos++;             
                tokens.push_back(token);  

                isBracketOpen--;
            } else {
                // If operator is not closed it's a invalid case
                if (isOperatorOpen == true) {
                    KUB_ERROR("[tokenize] operator is open, but we are pass all cases");
                    return std::nullopt;                
                }
            }
        }

        // If when loop is ended but some brackets are not closed it's invalid case
        if (isBracketOpen > 0) {
            KUB_ERROR("[tokenize] some bracket is not closed");
            return std::nullopt;
        }

        KUB_LEXER_DEBUG("[tokenize] finished, see next line for result");
        print(tokens);

        if (tokens.size() == 0) {
            KUB_ERROR("[tokenize] tokens size are zero");
            return std::nullopt;
        }

        return useShuntingYard ? shuntingYardAlgorithm(tokens) : tokens;
    }
}