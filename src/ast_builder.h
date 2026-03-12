#pragma once 
#include "ast.h"
#include "singleton.h"
#include "lexer.h"
#include "logger.h"

#include <stack>
#include <charconv>

namespace kubvc::algorithm {
    // TODO: Move create *NodeName* to ASTBuilder
    class ASTBuilder : public utility::Singleton<ASTBuilder> {
        public:
            bool build(ASTree& tree, const std::vector<Token>& tokens);
    };

    // TODO: Clear stack on errors or (invalid tree, nor sure) 
    inline bool ASTBuilder::build(ASTree& tree, const std::vector<Token>& tokens) {
        tree.clear();
        tree.createRoot();

        std::stack<std::shared_ptr<algorithm::INode>> nodeStack = { };

        for (const auto token : tokens) {
            switch (token.type) {
                case Token::Types::Number: {
                    const auto value = token.value;
                    double numberValue = 0.0;
                    auto result = std::from_chars(value.data(), value.data() + value.size(), numberValue).ec;
                    KUB_ASSERT(result == std::errc(), "Invalid convert");
                    const auto node = tree.createNumberNode(numberValue);
                    nodeStack.push(node);
                    break;
                }
                case Token::Types::Variable: {
                    const auto value = token.value;
                    const auto node = tree.createVariableNode(value.at(0));
                    nodeStack.push(node);
                    break;
                }
                case Token::Types::Operator: {
                    if (nodeStack.size() <= 1) {
                        KUB_ERROR("minimum size of stack is 2 for operator");
                        return false;
                    }
                    const auto arg1 = nodeStack.top();
                    nodeStack.pop();

                    const auto arg2 = nodeStack.top();
                    nodeStack.pop();

                    const auto value = token.value;
                    const auto node = tree.createOperatorNode(arg1, arg2, value.at(0));
                    nodeStack.push(node);
                    break;
                }
                case Token::Types::Function: {
                    if (nodeStack.size() == 0) {
                        KUB_ERROR("minimum size of stack is 1 for function");
                        return false;
                    }

                    const auto value = token.value;
                    // TODO: Args support, actually our function node is not supporting for multiple arguments
                    const auto node = tree.createFunctionNode(value);
                    node->argument = nodeStack.top();
                    nodeStack.pop();
                    nodeStack.push(node);
                    break;
                } 
                case Token::Types::UnaryOperator: {
                    if (nodeStack.size() == 0) {
                        KUB_ERROR("minimum size of stack is 1 for unary operator");
                        return false;
                    }

                    const auto value = token.value;
                    const auto top = nodeStack.top();
                    const auto node = tree.createUnaryOperatorNode(top, value.at(0));
                    nodeStack.pop();
                    nodeStack.push(node);
                    break;
                }                

                default:
                    KUB_ERROR("Unknown type or not implemented");
                    const auto node = tree.createInvalidNode("Invalid");
                    nodeStack.push(node);
                    break;
            }
        }

        tree.getRoot()->child = nodeStack.top();
        return tree.isValid();
    }

}