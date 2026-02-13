#include "expression_parser.h"
#include "math_base.h"
#include <charconv>

//#define KUB_ENABLE_PARSER_LOG_DEBUG
#ifdef KUB_ENABLE_PARSER_LOG_DEBUG
    #define KUB_PARSER_DEBUG(fmt, ...) KUB_DEBUG(fmt, ##__VA_ARGS__)
#else 
    #define KUB_PARSER_DEBUG(fmt, ...)
#endif

namespace kubvc::algorithm {
    Helpers::uchar Parser::getCurrentChar(const std::size_t& cursor, std::string_view text) {
        if (cursor > text.size()) {
            KUB_FATAL("Cursor is out of bounds");
            return '\0';
        }
        auto character = text[cursor];
        return static_cast<Helpers::uchar>(character);
    }

    std::string_view Parser::parseNumbers(std::size_t& cursor, std::string_view text) {
        const auto start = cursor;
        auto character = getCurrentChar(cursor, text);
        while(kubvc::algorithm::Helpers::isDigit(character) 
            || kubvc::algorithm::Helpers::isDot(character)) {
            cursor++;
            character = getCurrentChar(cursor, text);
        }
        return text.substr(start, cursor - start);
    }

    std::string_view Parser::parseLetters(std::size_t& cursor, std::string_view text, bool includeDigits) {
        const auto start = cursor;
        auto character = getCurrentChar(cursor, text);
        while(kubvc::algorithm::Helpers::isLetter(character) 
            || (kubvc::algorithm::Helpers::isDigit(character) 
            && includeDigits)) {
            cursor++;
            character = getCurrentChar(cursor, text);
        }
        return text.substr(start, cursor - start);
    }

    std::shared_ptr<INode> Parser::parseFunction(kubvc::algorithm::ASTree& tree, const std::size_t& startPos, 
        std::size_t& cursor, std::string_view text) {
        KUB_PARSER_DEBUG("Parse function...");
        auto funcPos = startPos;
        auto funcName = std::string(parseLetters(funcPos, text));
        // Convert text to lower case to avoid mismatch 
        kubvc::algorithm::Helpers::toLowerCase(funcName);

        if (funcPos > text.size())
            return nullptr;
    
        // Next should be bracket character
        auto brChar = getCurrentChar(funcPos, text);        

        // TODO: What if we are want support functions with more than one argument
        if (kubvc::algorithm::Helpers::isBracketStart(brChar)) {
            KUB_PARSER_DEBUG("So, is bracket found...");
            cursor++;

            auto argsNode = parseExpression(tree, text, cursor, true);
            auto argsNodeIsInvalid = argsNode->getType() == kubvc::algorithm::NodeTypes::Invalid;
            if (argsNode != nullptr && !argsNodeIsInvalid) {
                auto funcNode = tree.createFunctionNode(funcName);
                funcNode->argument = argsNode;
                return funcNode;            
            }
        }

        KUB_PARSER_DEBUG("Function parse failed...");
        return nullptr;
    }

    std::shared_ptr<INode> Parser::parseExplicitMultiplication(kubvc::algorithm::ASTree& tree, std::string_view text) {
        auto clearedNewText = std::string();
        char prevChar = '\0';
        
        for (char chr : text) {
            const auto currIsOperator = algorithm::Helpers::isOperator(chr);                
            if (currIsOperator) {
                break;
            }
            clearedNewText += chr;
        }

        auto newText = std::string();
        for (char chr : clearedNewText) {
            if (!newText.empty()) {
                const auto prevIsDigit = algorithm::Helpers::isDigit(prevChar);
                const auto currIsDigit = algorithm::Helpers::isDigit(chr);                
                const auto prevIsLetterOrUnderscore = kubvc::algorithm::Helpers::isLetter(prevChar) || prevChar == '_';                
                if ((prevIsDigit && !currIsDigit && std::isalpha(chr)) || 
                    (prevIsLetterOrUnderscore && (currIsDigit || std::isalpha(chr)))) {
                    newText += '*';
                }
            }
            
            newText += chr;
            prevChar = chr;
        }

        std::size_t newCursor = 0;
        auto expr = parseExpression(tree, newText, newCursor, false);        
        KUB_PARSER_DEBUG("newText: {} cursor: {}", newText.c_str(), newCursor);
        if (expr == nullptr || expr->getType() == kubvc::algorithm::NodeTypes::Invalid) {
            return tree.createInvalidNode("InvalidElementExpMult");
        }
        return expr;
    }   
           
    std::shared_ptr<INode> Parser::parseElement(kubvc::algorithm::ASTree& tree, std::string_view text, std::size_t& cursor, 
        char currentChar, bool isSubExpression) {
        KUB_PARSER_DEBUG("[parseElement] -> {}", currentChar); 

        // Skip white space if we are find it  
        if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar)) {
            KUB_PARSER_DEBUG("[parseElement] Ignore white space in parse element stage"); 
            cursor++;
            currentChar = getCurrentChar(cursor, text);
        } 

        if (kubvc::algorithm::Helpers::isDigit(currentChar)) {
            KUB_PARSER_DEBUG("[parseElement] isDigit {}", currentChar);
            std::size_t startCursor = cursor;
            auto out = parseLetters(cursor, text, true);
            KUB_ASSERT(!out.empty(), "Parse number has a empty output");
            if (kubvc::algorithm::Helpers::isNumber(out)) {
                KUB_PARSER_DEBUG("[parseElement] isNumber {}", out.data());
                double result = 0.0;
                auto convertResult = std::from_chars(out.data(), out.data() + out.size(), result).ec;
                KUB_ASSERT(convertResult == std::errc(), "Invalid convert");
                return tree.createNumberNode(result); 
            } 
            else {
                KUB_PARSER_DEBUG("[parseElement] parseExplicitMultiplication {}", out.data());
                return parseExplicitMultiplication(tree, text.substr(startCursor, cursor));
            }
        }    
        else if (kubvc::algorithm::Helpers::isLetter(currentChar)) {
            auto out = parseLetters(cursor, text);
            KUB_PARSER_DEBUG("[parseElement] Is letter | parsed {}", out.data());

            const auto outSize = out.size();
            KUB_ASSERT(outSize != 0, "Output has a zero size");                        
            // Find a constant name in container, if found it, we are create number node with constant value 
            auto constResult = math::containers::Constants.get(out);
            if (constResult.has_value()) {
                KUB_PARSER_DEBUG("[parseElement] Is constant, create number node with constant value");
                return tree.createNumberNode(constResult.value());
            }

            // If output size is bigger than one it's a probably a function  
            if (outSize > 1) {
                auto function = parseFunction(tree, cursor - outSize, cursor, text);
                if (function == nullptr) {
                    return parseExplicitMultiplication(tree, text.substr(cursor - outSize, cursor));
                } 
                else {
                    return function;
                }
            }

            // If output size is single character it's a variable 
            return tree.createVariableNode(currentChar);
            
        }
        else if (kubvc::algorithm::Helpers::isBracketStart(currentChar))  {
            KUB_PARSER_DEBUG("[parseElement] Bracket start");
            cursor++;
            return parseExpression(tree, text, cursor, true);
        } 
        else if (kubvc::algorithm::Helpers::isUnaryOperator(currentChar)) {
            KUB_PARSER_DEBUG("[parseElement] Possible unary operator");
            cursor++;
            auto node = parseExpression(tree, text, cursor, isSubExpression);
            if (node == nullptr) {
                return tree.createInvalidNode("InvalidElement");
            }

            cursor--;
            return tree.createUnaryOperatorNode(node, currentChar);
        }

        KUB_WARN("Invalid | {} | cursor: %d | text:{}", currentChar, cursor, text.data());
        return tree.createInvalidNode("InvalidElement");
    }

    std::shared_ptr<INode> Parser::parseExpression(kubvc::algorithm::ASTree& tree, std::string_view text, 
        std::size_t& cursor, bool isSubExpression) {
        const auto textSize = text.size();
        // Don't do anything if text is empty 
        if (textSize == 0)
            return nullptr;

        KUB_PARSER_DEBUG("parseExpression | Start | cursor: %d", cursor);
        auto left = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);
        while (true) {
            auto currentChar = getCurrentChar(cursor, text);  
            KUB_PARSER_DEBUG("Current character in expression cycle: {}", currentChar); 

            // Ignore white spaces
            if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar)) {
                KUB_PARSER_DEBUG("Ignore white space in expression parse"); 
                cursor++;
                continue;
            }  
            // We are want to continue cycle or want to break it if it's a subexpression
            else if (kubvc::algorithm::Helpers::isBracketEnd(currentChar)) {
                KUB_PARSER_DEBUG("End of bracket | isSubExpression:{}", isSubExpression);
                cursor++;

                if (isSubExpression) {
                    KUB_PARSER_DEBUG("Return left node");
                    return left;
                }

                continue;
            }
            // If current character is blank we are break cycle
            else if (!currentChar) {
                KUB_PARSER_DEBUG("parseExpression | End | Leave from cycle");
                break;
            }
            // Avoid inf cycle caused by if last character is operator, parser will be think it's unary and gets stuck 
            else if (kubvc::algorithm::Helpers::isOperator(currentChar) && cursor == (textSize - 1)) {
                return nullptr;
            }

            // Augment cursor position
            cursor++;
            // Try to find element 
            auto right = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);
            if (right == nullptr) {
                KUB_WARN("parseElement is returned a nullptr, maybe something not implemented, check logs.");                     
                return nullptr;
            }

            left = tree.createOperatorNode(left, right, currentChar);
        }   

        // If we are actually leave from cycle and if isSubExpression is true, 
        // it means we are not found end brecket symbol, so it's a invalid expression   
        if (isSubExpression) {
            // In some cases we are reached from text range by one character, so it can be end of bracket. 
            if (cursor > textSize) {
                KUB_WARN("Edgy case found");
                auto preLastChar = getCurrentChar(cursor - 1, text);
                if (kubvc::algorithm::Helpers::isBracketEnd(preLastChar)) {
                    return left;    
                }
            }

            KUB_WARN("is invalid brecket");
            return tree.createInvalidNode("InvalidBrecket");
        }

        return left;
    }

    void Parser::parse(kubvc::algorithm::ASTree& tree, std::string_view text, const std::size_t cursor_pos) {
        if (text.size() == 0) {
            tree.clear();
            tree.createRoot();
            return;
        }

        KUB_PARSER_DEBUG("\n\nRun text parser...");
        KUB_PARSER_DEBUG("----------------------------------------");
       
        std::size_t cursor = cursor_pos;
        auto root = tree.getRoot();
        root->child = parseExpression(tree, text, cursor, false);
        KUB_PARSER_DEBUG("----------------------------------------");
    }
}