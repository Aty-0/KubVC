#pragma once 
#include "ast.h"
#include "singleton.h"
#include "lexer.h"
#include "logger.h"

#include <stack>
#include <charconv>

namespace kubvc::algorithm {
    class ASTBuilder : public utility::Singleton<ASTBuilder> {
        public:
            bool build(ASTree& tree, const std::vector<Token>& tokens);

        private:        
            [[nodiscard]] NodePtr<NodeTypes::Root> createRoot(std::shared_ptr<INode> child) const;
            [[nodiscard]] NodePtr<NodeTypes::Variable> createVariableNode(char value) const;
            [[nodiscard]] NodePtr<NodeTypes::Number> createNumberNode(double value) const;
            [[nodiscard]] NodePtr<NodeTypes::Operator> createOperatorNode(std::shared_ptr<INode> x,  std::shared_ptr<INode> y, char op) const;
            [[nodiscard]] NodePtr<NodeTypes::UnaryOperator> createUnaryOperatorNode(std::shared_ptr<INode> x, char op) const;
            [[nodiscard]] NodePtr<NodeTypes::Invalid> createInvalidNode(std::string_view name) const;
            [[nodiscard]] NodePtr<NodeTypes::Function> createFunctionNode(std::string_view name) const;

            template <NodeTypes NodeType>
            [[nodiscard]] NodePtr<NodeType> createNode() const;

    };

    template <NodeTypes NodeType>
    inline NodePtr<NodeType> ASTBuilder::createNode() const {
        static std::uint32_t id = 0;
        const auto node = std::make_shared<NodeTraits<NodeType>>();
        node->setId(id);
        id++;
        return node;
    }

    inline NodePtr<NodeTypes::Root> ASTBuilder::createRoot(std::shared_ptr<INode> child) const {
        const auto node = createNode<NodeTypes::Root>();
        node->child = child;
        return node;
    }

    inline NodePtr<NodeTypes::Variable> ASTBuilder::createVariableNode(char value) const {
        const auto node = createNode<NodeTypes::Variable>();
        node->setValue(value);    
        return node;
    }

    inline NodePtr<NodeTypes::Number> ASTBuilder::createNumberNode(double value) const {
        const auto node = createNode<NodeTypes::Number>();
        node->setValue(value);   
        return node;
    }

    inline NodePtr<NodeTypes::Operator> ASTBuilder::createOperatorNode(std::shared_ptr<INode> x, 
        std::shared_ptr<INode> y, char op) const {
        const auto node = createNode<NodeTypes::Operator>();
        node->operation = op;
        node->left = std::move(x);
        node->right = std::move(y);
        return node;
    }

    inline NodePtr<NodeTypes::UnaryOperator> ASTBuilder::createUnaryOperatorNode(std::shared_ptr<kubvc::algorithm::INode> x, char op) const {
        const auto node = createNode<NodeTypes::UnaryOperator>();
        node->operation = op;
        node->child = std::move(x);
        return node;
    }

    inline NodePtr<NodeTypes::Invalid> ASTBuilder::createInvalidNode(std::string_view name) const {
        const auto node = createNode<NodeTypes::Invalid>();
        node->name = name;
        return node;
    }

    inline NodePtr<NodeTypes::Function> ASTBuilder::createFunctionNode(std::string_view name) const {
        const auto node = createNode<NodeTypes::Function>();
        node->name = name;
        return node;
    }

    inline bool ASTBuilder::build(ASTree& tree, const std::vector<Token>& tokens) {
        tree.clear();

        std::stack<std::shared_ptr<algorithm::INode>> nodeStack = { };
        for (const auto token : tokens) {
            if (token.value.empty()) {
                KUB_ERROR("token with type {} has empty value, skip", static_cast<std::int32_t>(token.type));
                continue;
            }
            
            switch (token.type) {
                case Token::Types::Number: {
                    const auto value = token.value;
                    double numberValue = 0.0;
                    const auto result = std::from_chars(value.data(), value.data() + value.size(), numberValue).ec;
                    KUB_ASSERT(result == std::errc(), "Invalid convert");
                    const auto node = createNumberNode(numberValue);
                    nodeStack.push(node);
                    break;
                }
                case Token::Types::Variable: {
                    const auto value = token.value;
                    const auto node = createVariableNode(value.at(0));
                    nodeStack.push(node);
                    break;
                }
                case Token::Types::Operator: {
                    if (nodeStack.size() <= 1) {
                        KUB_ERROR("minimum size of stack is 2 for operator");
                        
                        // Clear node
                        if (!nodeStack.empty()) {
                            nodeStack.pop();
                        }
                        return false;
                    }
                    const auto arg1 = nodeStack.top();
                    nodeStack.pop();

                    const auto arg2 = nodeStack.top();
                    nodeStack.pop();

                    const auto value = token.value;
                    const auto node = createOperatorNode(arg2, arg1, value.at(0));
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
                    const auto node = createFunctionNode(value);
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
                    const auto node = createUnaryOperatorNode(top, value.at(0));
                    nodeStack.pop();
                    nodeStack.push(node);
                    break;
                }                

                default: {
                    KUB_ERROR("Unknown type or not implemented");
                    const auto node = createInvalidNode("Invalid");
                    nodeStack.push(node);
                    break;
                }
            }
        }
        const auto root = createRoot(nodeStack.top());
        tree.setRoot(root);
        nodeStack.pop();

        if (nodeStack.size() > 0) {
            KUB_WARN("Node stack is not empty on build end, clear...");
            while(!nodeStack.empty()) {
                nodeStack.pop();
            }
        }

        return tree.validate();
    }

}