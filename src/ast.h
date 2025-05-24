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
        UnaryOperator,           
        Invalid,           
    };

    struct Node 
    {
        ~Node();
        inline virtual auto getType() -> NodeTypes const { return NodeTypes::None; } 
        inline virtual void calculate(const double& n, double& result) { }
        
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

    enum class Operators 
    {
        Plus, 
        Minus,
        Multiplication,
        Division,
        Power,
        Equal,
        Module,
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
            case '%':
                return Operators::Module;
        }
        return Operators::Unknown;
    } 

    struct UnaryOperatorNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::UnaryOperator; }

        char operation;
        std::shared_ptr<Node> child; 
        
        inline virtual void calculate(const double& n, double& result) final
        {
            if (child == nullptr || child->getType() == NodeTypes::Invalid)
                return;

            child->calculate(n, result);
            
            auto op = getOperatorFrom(operation);
            switch(op)
            {
                case Operators::Plus:
                    result = std::fabs(result);
                    break;
                case Operators::Minus:
                    result = -result;
                    break;
            }
        }
    };

    // Operator can store left and right node links
    struct OperatorNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Operator; }        

        char operation;
        std::shared_ptr<Node> right; 
        std::shared_ptr<Node> left;

        inline virtual void calculate(const double& n, double& result) final
        {
          if ((right == nullptr || left == nullptr) 
                || (right->getType() == NodeTypes::Invalid 
                || left->getType() == NodeTypes::Invalid))
                return;

            double firstResult, secondResult = 0.0;
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
                    if (std::abs(secondResult) < 1e-10) 
                    {  
                        result = std::numeric_limits<double>::quiet_NaN();
                        break;
                    }
                    
                    result = firstResult / secondResult;
                    break;
                }
                case Operators::Module:
                    result = std::fmod(firstResult, secondResult);
                    break;
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
                case NodeTypes::UnaryOperator:
                case NodeTypes::Number:
                {
                    double argumentResult = 0;
                    argument->calculate(n, argumentResult);
                    result = Helpers::computeFunction(name, argumentResult); 
                    break;
                }
                case NodeTypes::Variable:
                    result = Helpers::computeFunction(name, n); 
                    break;
            }
        }
    };
    
    class ASTree
    {
        public:
            ASTree();    
            
            void clear();
            void createRoot();
            
            // Is tree had any invalid node 
            inline bool isValid() const { return isValidFrom(m_root); }

            // Is tree had any invalid node but we are start from specified node 
            bool isValidFrom(std::shared_ptr<kubvc::algorithm::Node> start) const;

            inline std::shared_ptr<RootNode> getRoot() const { return m_root; }

            std::shared_ptr<kubvc::algorithm::VariableNode> createVariableNode(char value);
            std::shared_ptr<kubvc::algorithm::NumberNode> createNumberNode(double value);
            std::shared_ptr<kubvc::algorithm::OperatorNode> createOperatorNode(std::shared_ptr<kubvc::algorithm::Node> x,  std::shared_ptr<kubvc::algorithm::Node> y, char op);
            std::shared_ptr<kubvc::algorithm::UnaryOperatorNode> createUnaryOperatorNode(std::shared_ptr<kubvc::algorithm::Node> x, char op);
            std::shared_ptr<kubvc::algorithm::InvalidNode> createInvalidNode(const std::string& name);
            std::shared_ptr<kubvc::algorithm::FunctionNode> createFunctionNode(const std::string& name);


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

            static inline std::string getNodeName(const kubvc::algorithm::NodeTypes& type)
            {
                switch (type)
                {
                    case kubvc::algorithm::NodeTypes::None:
                        return "None";           
                    case kubvc::algorithm::NodeTypes::Root:
                        return "Root";           
                    case kubvc::algorithm::NodeTypes::Number: 
                        return "Number";           
                    case kubvc::algorithm::NodeTypes::Variable:
                        return "Variable";           
                    case kubvc::algorithm::NodeTypes::Function:
                        return "Function";           
                    case kubvc::algorithm::NodeTypes::Operator:
                        return "Operator";               
                }
            
                return "Unknown";
            }

        private:
            std::shared_ptr<RootNode> m_root;
    };
}