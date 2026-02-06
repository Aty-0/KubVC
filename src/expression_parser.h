#pragma once
#include "ast.h"
#include "logger.h"
#include "singleton.h"

namespace kubvc::algorithm {
    using uchar = unsigned char;
    class Parser : public utility::Singleton<Parser>
    {
        public:            
            auto parse(kubvc::algorithm::ASTree& tree, std::string_view text, const std::size_t cursor_pos = 0) -> void;

        private:
            auto parseExpression(kubvc::algorithm::ASTree& tree, std::string_view text, std::size_t& cursor, bool isSubExpression) -> std::shared_ptr<INode>;
            auto parseElement(kubvc::algorithm::ASTree& tree, std::string_view text, std::size_t& cursor, char currentChar, bool isSubExpression) -> std::shared_ptr<INode>;
            auto parseFunction(kubvc::algorithm::ASTree& tree, const std::size_t& cursor_pos, std::size_t& cursor, std::string_view text) -> std::shared_ptr<INode>;                        
            auto parseExplicitMultiplication(kubvc::algorithm::ASTree& tree, std::string_view text) -> std::shared_ptr<INode>;
            auto parseLetters(std::size_t& cursor, std::string_view text, bool includeDigits = true) -> std::string_view;            
            auto parseNumbers(std::size_t& cursor, std::string_view text) -> std::string_view;
            
            auto getCurrentChar(const std::size_t& cursor, std::string_view text) -> uchar;
    };
}