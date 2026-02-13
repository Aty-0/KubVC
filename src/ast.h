#pragma once
#include <memory>
#include <string>
#include <functional>
#include <glm/glm.hpp>

#include "logger.h"
#include "alg_helpers.h"

namespace kubvc::algorithm {
    enum class NodeTypes {
        None,
        Root,
        Number, 
        Variable,
        Function,
        Operator,           
        UnaryOperator,           
        Invalid,           
    };

    enum class Operators {
        Plus, 
        Minus,
        Multiplication,
        Division,
        Power,
        Equal,
        Module,
        Unknown,
    };

    [[nodiscard]] static inline Operators getOperatorFrom(Helpers::uchar chr) {   
        switch (chr) {
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

    struct INode {
        virtual NodeTypes getType() const = 0;
        virtual void calculate(const double& n, double& result) = 0;
        
        inline std::int32_t getId() const { return m_id; }
        inline void setId(std::uint32_t id) { if (m_id == DEFAULT_NODE_ID) { m_id = id; } }        

        private:
            static constexpr auto DEFAULT_NODE_ID = -1;
            std::int32_t m_id = DEFAULT_NODE_ID; 
    };

    template <typename ValueType>
    struct NodeValue {
        inline ValueType getValue() const { return m_value; }
        inline void setValue(const ValueType& value) { m_value = value; }
        protected:        
            ValueType m_value;
    };

    template<NodeTypes NodeType> 
    struct NodeTraits { };
    
    template<>
    struct NodeTraits<NodeTypes::Root> : INode {
        virtual NodeTypes getType() const final { return NodeTypes::Root; }
        virtual void calculate(const double& n, double& result) { 
            if (child == nullptr)
                return;
            child->calculate(n, result);            
        }

        std::shared_ptr<INode> child;
    };

    template<>
    struct NodeTraits<NodeTypes::Variable> : INode, NodeValue<char> { 
        virtual NodeTypes getType() const final { return NodeTypes::Variable; }
        virtual void calculate(const double& n, double& result) final { result = n; }
    };

    template<>
    struct NodeTraits<NodeTypes::Number> : INode, NodeValue<double> {
        virtual NodeTypes getType() const final { return NodeTypes::Number; }
        virtual void calculate(const double& n, double& result) final { result = m_value; }
    };

    template<>
    struct NodeTraits<NodeTypes::Invalid> : INode {
        virtual NodeTypes getType() const final { return NodeTypes::Invalid; }
        virtual void calculate(const double& n, double& result) final { } // Do nothing

        std::string name;
    };

    template<>
    struct NodeTraits<NodeTypes::UnaryOperator> : INode {        
        virtual NodeTypes getType() const final { return NodeTypes::UnaryOperator; }
        virtual void calculate(const double& n, double& result) final;
        
        char operation;
        std::shared_ptr<INode> child; 
    };

    template<>
    struct NodeTraits<NodeTypes::Operator> : INode {
        virtual NodeTypes getType() const final { return NodeTypes::Operator; }
        virtual void calculate(const double& n, double& result) final;        
        
        char operation;
        std::shared_ptr<INode> right; 
        std::shared_ptr<INode> left;
    };

    template<>
    struct NodeTraits<NodeTypes::Function> : INode {
        virtual NodeTypes getType() const final { return NodeTypes::Function; }
        virtual void calculate(const double& n, double& result) final;
        
        std::string name;
        std::shared_ptr<INode> argument;
    };

    inline static std::string_view getNodeName(const kubvc::algorithm::NodeTypes& type) {
        switch (type) {
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
    
    template<NodeTypes NodeType>
    using NodePtr = std::shared_ptr<NodeTraits<NodeType>>;

    template <NodeTypes Type>
    inline static std::shared_ptr<INode> castToINodePtr(NodePtr<Type> ptr) { return std::static_pointer_cast<INode>(ptr); }

    template <NodeTypes Type>
    inline static NodePtr<Type> castToNodePtr(std::shared_ptr<INode> ptr) { return std::dynamic_pointer_cast<NodeTraits<Type>>(ptr); }

    class ASTree {
        public:                    
            void clear();
            void createRoot();
        
            NodePtr<NodeTypes::Variable> createVariableNode(char value);
            NodePtr<NodeTypes::Number> createNumberNode(double value);
            NodePtr<NodeTypes::Operator> createOperatorNode(std::shared_ptr<INode> x,  std::shared_ptr<INode> y, char op);
            NodePtr<NodeTypes::UnaryOperator> createUnaryOperatorNode(std::shared_ptr<INode> x, char op);
            NodePtr<NodeTypes::Invalid> createInvalidNode(std::string_view name);
            NodePtr<NodeTypes::Function> createFunctionNode(std::string_view name);

            // Start validating ast from root node
            inline bool isValid() const { return isValidFrom(castToINodePtr<NodeTypes::Root>(m_root)); }

            // Start validating ast from specific node
            bool isValidFrom(std::shared_ptr<INode> start) const;

            inline NodePtr<NodeTypes::Root> getRoot() const { return m_root; }

        private:
            NodePtr<NodeTypes::Root> m_root;

            // Create node object 
            template <NodeTypes NodeType>
            NodePtr<NodeType> createNode() const;
    };
}

#include "ast_inline.h"