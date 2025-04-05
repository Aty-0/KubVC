#pragma once
#include "ast.h"
#include "logger.h"

namespace kubvc::algorithm
{
    class Parser
    {
        using NodePtr = std::shared_ptr<kubvc::algorithm::Node>;
        using uchar = unsigned char;
        public:            
            static void parse(kubvc::algorithm::ASTree& tree, const std::string& text, const std::size_t cursor_pos = 0);

        private:
            static NodePtr parseExpression(kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, bool isSubExpression);
            static NodePtr parseElement(kubvc::algorithm::ASTree& tree, const std::string& text, std::size_t& cursor, char currentChar, bool isSubExpression);
            static NodePtr parseFunction(kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, std::size_t& cursor, const std::string& text);            
            
            static std::string parseLetters(std::size_t& cursor, const std::string& text, bool includeDigits = true);            
            static std::string parseNumbers(std::size_t& cursor, const std::string& text);
            
            
            static inline uchar getCurrentChar(const std::size_t& cursor, const std::string& text)
            {
                if (cursor > text.size())
                {
                    FATAL("Cursor is out of bounds");
                    return '\0';
                }
                auto character = text[cursor];

                return static_cast<uchar>(character);
            }
    };
}