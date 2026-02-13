#pragma once
#include "ast.h"
#include "logger.h"
#include "singleton.h"

namespace kubvc::algorithm {
    class Parser : public utility::Singleton<Parser> {
        public:            
            void parse(kubvc::algorithm::ASTree& tree, std::string_view text, const std::size_t cursor_pos = 0);

        private:
            std::shared_ptr<INode> parseExpression(kubvc::algorithm::ASTree& tree, std::string_view text, std::size_t& cursor, bool isSubExpression);
            std::shared_ptr<INode> parseElement(kubvc::algorithm::ASTree& tree, std::string_view text, std::size_t& cursor, char currentChar, bool isSubExpression);
            std::shared_ptr<INode> parseFunction(kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, std::size_t& cursor, std::string_view text);                        
            std::shared_ptr<INode> parseExplicitMultiplication(kubvc::algorithm::ASTree& tree, std::string_view text);
            std::string_view parseLetters(std::size_t& cursor, std::string_view text, bool includeDigits = true);            
            std::string_view parseNumbers(std::size_t& cursor, std::string_view text);
            
            Helpers::uchar getCurrentChar(const std::size_t& cursor, std::string_view text);
    };
}