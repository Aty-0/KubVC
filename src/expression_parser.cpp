#include "expression_parser.h"

namespace kubvc::algorithm
{
    std::string Parser::parseNumbers(std::size_t& cursor, const std::string& text)
    {
        char character = getCurrentChar(cursor, text);
        std::string output = std::string();
        while(kubvc::algorithm::Helpers::isDigit(character) 
            || kubvc::algorithm::Helpers::isDot(character))
        {
            output += character;
            cursor++;
            character = getCurrentChar(cursor, text);
        }
        return output;
    }

    std::string Parser::parseLetters(std::size_t& cursor, const std::string& text, bool includeDigits)
    {
        char character = getCurrentChar(cursor, text);
        std::string output = std::string();
        while(kubvc::algorithm::Helpers::isLetter(character) || (kubvc::algorithm::Helpers::isDigit(character) && includeDigits))
        {
            output += character;
            cursor++;
            character = getCurrentChar(cursor, text);
        }
        return output;
    }

    Parser::NodePtr Parser::parseFunction(kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, std::size_t& cursor, const std::string& text)
    {
        std::size_t funcCursor = cursor_pos;
        std::string funcName = std::string();

        funcName = parseLetters(funcCursor, text);

        // Convert text to lower case to avoid mismatch 
        kubvc::algorithm::Helpers::toLowerCase(funcName);

        if (funcCursor > text.size())
            return tree.createInvalidNode(text);
    
        // Next should be bracket character
        auto brChar = getCurrentChar(funcCursor, text);        

        // TODO: What if we are want support functions with more than one argument
        if (kubvc::algorithm::Helpers::isBracketStart(brChar))
        {
            DEBUG("So, is bracket found...");
            cursor++;

            auto argsNode = parseExpression(tree, text, cursor, true);
            if (argsNode->getType() != kubvc::algorithm::NodeTypes::Invalid)
            {
                auto funcNode = tree.createFunctionNode(funcName);
                funcNode->argument = argsNode;
                return funcNode;            
            }

            WARN("Bad node returned...");
        }

        return tree.createInvalidNode(text);
    }


    Parser::NodePtr Parser::parseElement(kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, char currentChar, bool isSubExpression)
    {
        DEBUG("try to find something usefull for %c", currentChar); 

        // Skip white space if we are find it  
        if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar))
        {
            DEBUG("Ignore white space in parse element stage"); 
            cursor++;
            currentChar = getCurrentChar(cursor, text);
        } 

        if (kubvc::algorithm::Helpers::isDigit(currentChar))
        {
            auto out = parseNumbers(cursor, text);

            DEBUG("Is digit %s", out.c_str());
            if (out.empty())
            {
                ERROR("Parse number has a empty output, hmm. Ignore!");
            }
            else
            {
                if (kubvc::algorithm::Helpers::isNumber(out))
                {
                    return tree.createNumberNode(std::atof(out.c_str()));                
                }
                else
                {
                    ERROR("Output is actually not a number, so ignore him! We catch that bad guy: %s", out.c_str());
                }
            }
        }    
        else if (kubvc::algorithm::Helpers::isLetter(currentChar))
        {
            auto out = parseLetters(cursor, text);
            DEBUG("Is letter | parsed %s", out.c_str());
            const auto outSize = out.size();
            if (outSize == 0)
            {
                ERROR("Output has a zero size");                
            } 
            else if (outSize > 1)
            {
                DEBUG("Parse function...");
                return parseFunction(tree, cursor - outSize, cursor, text);
            }
            else 
            {
                // TODO: What we need to do with constants 
                return tree.createVariableNode(currentChar);
            }
        }
        else if (kubvc::algorithm::Helpers::isBracketStart(currentChar)) 
        {
            cursor++;
            DEBUG("Bracket start");
            auto node = parseExpression(tree, text, cursor, true);
            return node;
        } 
        else if (kubvc::algorithm::Helpers::isUnaryOperator(currentChar))
        {
            DEBUG("Possible unary operator");
            cursor++;
            auto node = parseExpression(tree, text, cursor, isSubExpression);

            cursor--;
            return tree.createUnaryOperatorNode(node, currentChar);
        }

        DEBUG("Nothing found");

        return tree.createInvalidNode("INV_NODE");
    }

    Parser::NodePtr Parser::parseExpression(kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, bool isSubExpression)
    {
        // Don't do anything if text is empty 
        if (text.size() == 0)
            return nullptr;

        DEBUG("----------------------------------------");
        DEBUG("parseExpression | Start | cursor: %d", cursor);

        std::shared_ptr<kubvc::algorithm::Node> left = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);

        while (true)
        {
            auto currentChar = getCurrentChar(cursor, text);  
            DEBUG("Current character in expression cycle: %c", currentChar); 

            // Ignore white spaces
            if (kubvc::algorithm::Helpers::isWhiteSpace(currentChar))
            {
                DEBUG("Ignore white space in expression parse"); 
                cursor++;
                continue;
            }  
            // We are want to continue cycle or want to break it if it's a subexpression
            else if (kubvc::algorithm::Helpers::isBracketEnd(currentChar))
            {
                DEBUG("End of bracket | isSubExpression:%i", isSubExpression);
                cursor++;

                if (isSubExpression)
                {
                    DEBUG("Return left node");
                    return left;
                }

                continue;
            }
            // If current character is not operator we are leave from cycle 
            else if (!kubvc::algorithm::Helpers::isOperator(currentChar))
            {
                DEBUG("parseExpression | End | Leave from cycle");
                DEBUG("----------------------------------------");
                break;
            }

            // Augment cursor position 
            cursor++;  
            // Try to find something 
            auto right = parseElement(tree, text, cursor, getCurrentChar(cursor, text), isSubExpression);
            if (right == nullptr)
            {
                ERROR("parseElement is returned nullptr, maybe syntax error or not implemented element");
                return nullptr;
            }

            left = tree.createOperatorNode(left, right, currentChar);
        }   

        // If we are actually leave from cycle and if isSubExpression is true, 
        // it means we are not found end brecket symbol, so it's a invalid expression   
        if (isSubExpression)
        {
            // In some cases we are reached from text range by one character, so it can be end of bracket. 
            if (cursor > text.size())
            {
                WARN("Edgy case found");
                auto preLastChar = getCurrentChar(cursor - 1, text);
                if (kubvc::algorithm::Helpers::isBracketEnd(preLastChar))
                {
                    return left;    
                }
            }

            WARN("is invalid brecket");
            return tree.createInvalidNode("InvalidBrecket");
        }

        return left;
    }

    void Parser::parse(kubvc::algorithm::ASTree& tree, const std::string& text, const std::size_t cursor_pos)
    {
        std::size_t cursor = cursor_pos;
        auto root = static_cast<kubvc::algorithm::RootNode*>(tree.getRoot().get());
        root->child = parseExpression(tree, text, cursor, false);
    }
}