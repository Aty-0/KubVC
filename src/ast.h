#pragma once
#include <memory>
#include <string>
#include <functional>
#include "logger.h"
#include "alg_helpers.h"

namespace kubvc::algorithm
{
    enum class NodeTypes
    {
        None,
        Root,
        Number, 
        Variable,
        Function,
        Operator,           
        Invalid,           
    };

    struct Node 
    {
        ~Node();
        inline virtual auto getType() -> NodeTypes const { return NodeTypes::None; } 
        inline virtual void calculate(const double& n, double& result) { }
        
        std::shared_ptr<Node> parent;

        // TODO: get set (set only if id is -1)
        std::int32_t id = -1; 
    };
    
    struct RootNode : Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Root; }
        inline virtual void calculate(const double& n, double& result)
        { 
            if (child == nullptr)
                return;
            child->calculate(n, result);            
        }
        
        std::shared_ptr<Node> child;
    };

    template <typename Type>
    struct NodeWithValue : Node
    {        
        Type value;
    };

    struct VariableNode : public NodeWithValue<std::string> 
    { 
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Variable; }        
        inline virtual void calculate(const double& n, double& result) final { result = n; }
    };

    // TODO: Make nodes with int and float? double? 
    struct NumberNode : public NodeWithValue<double>
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Number; }
        
        inline virtual void calculate(const double& n, double& result) final { result = value; }
    };

    struct InvalidNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Invalid; }
        std::string name;
    };

    // Operator can store left and right node links
    struct OperatorNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Operator; }
        
        enum class Operators 
        {
            Plus, 
            Minus,
            Multiplication,
            Division,
            Power,
            Equal,
            Unknown,
        };
        
        [[nodiscard]] static inline Operators getOperatorFrom(unsigned char chr)
        {   
            switch (chr)
            {
                case '+':
                    return Operators::Plus;
                case '-':
                    return Operators::Minus;
                case '*':
                    return Operators::Multiplication;
                case '/':
                    return Operators::Division;
                case '=':
                    return Operators::Equal;
                case '^':
                    return Operators::Power;
            }
            return Operators::Unknown;
        } 

        char operation;
        std::shared_ptr<Node> right; 
        std::shared_ptr<Node> left;

        inline virtual void calculate(const double& n, double& result) final
        {
          if ((right == nullptr || left == nullptr) 
                || (right->getType() == NodeTypes::Invalid 
                || left->getType() == NodeTypes::Invalid))
                return;

            double firstResult, secondResult = 0;
            left->calculate(n, firstResult);
            right->calculate(n, secondResult);

            auto op = getOperatorFrom(operation);
            switch(op)
            {
                case Operators::Plus:
                    result = firstResult + secondResult;
                    break;
                case Operators::Minus:
                    result = firstResult - secondResult;
                    break;
                case Operators::Multiplication:
                    result = firstResult * secondResult;
                    break;
                case Operators::Division:
                {
                    if (secondResult == 0)
                    {
                        ERROR("[OperatorNode] second node is 0, we can't do division, result is NAN now!");
                        result = NAN;
                        break;   
                    }
                    result = firstResult / secondResult;
                    break;
                }
                case Operators::Power:
                    result = std::pow(firstResult, secondResult);
                    break;
            }              
        }
    };

    struct FunctionNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Function; }
        
        std::string name;
        std::shared_ptr<Node> argument;

        inline virtual void calculate(const double& n, double& result) final 
        {
            if (argument == nullptr)
            {
                ERROR("[FunctionNode] Argument is null");
                return;
            }
            
            switch (argument->getType())
            {
                case NodeTypes::Operator:
                case NodeTypes::Function:
                case NodeTypes::Number:
                {
                    double argumentResult = 0;
                    argument->calculate(n, argumentResult);
                    result = Helpers::getResultFromFunction(name, argumentResult); 
                    break;
                }
                case NodeTypes::Variable:
                    result = Helpers::getResultFromFunction(name, n); 
                    break;
            }
        }
    };
    
    class ASTree
    {
        public:
            ASTree();    
            
            void clear();
            void makeRoot();
            
            inline std::shared_ptr<Node> getRoot() const { return m_root; }

            template <typename T, typename = std::enable_if<std::is_same<T, Node>::value 
                || std::is_base_of<T, Node>::value>>
            inline std::shared_ptr<T> createNode() const
            {
                auto node = std::make_shared<T>();
                static std::int32_t id = 0;
                node->id = id;
                id++;
                return node;
            }

        private:
            std::shared_ptr<Node> m_root;
    };
}