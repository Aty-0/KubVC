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

    [[nodiscard]] static inline Operators getOperatorFrom(unsigned char chr) {   
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
        virtual auto getType() const -> NodeTypes = 0;
        virtual auto calculate(const double& n, double& result) -> void = 0;
        
        inline auto getId() const -> std::int32_t { return m_id; }
        inline auto setId(std::uint32_t id) -> void { if (m_id == DEFAULT_NODE_ID) { m_id = id; } }        

        private:
            static constexpr auto DEFAULT_NODE_ID = -1;
            std::int32_t m_id = DEFAULT_NODE_ID; 
    };

    template <typename ValueType>
    struct NodeValue {
        inline auto getValue() const -> ValueType { return m_value; }
        inline auto setValue(const ValueType& value) -> void { m_value = value; }
        protected:        
            ValueType m_value;
    };

    template<NodeTypes NodeType> 
    struct NodeTraits { };
    
    template<>
    struct NodeTraits<NodeTypes::Root> : INode {
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Root; }
        virtual auto calculate(const double& n, double& result) -> void { 
            if (child == nullptr)
                return;
            child->calculate(n, result);            
        }

        std::shared_ptr<INode> child;
    };

    template<>
    struct NodeTraits<NodeTypes::Variable> : INode, NodeValue<char> { 
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Variable; }
        virtual auto calculate(const double& n, double& result) -> void final { result = n; }
    };

    template<>
    struct NodeTraits<NodeTypes::Number> : INode, NodeValue<double> {
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Number; }
        virtual auto calculate(const double& n, double& result) -> void final { result = m_value; }
    };

    template<>
    struct NodeTraits<NodeTypes::Invalid> : INode {
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Invalid; }
        virtual auto calculate(const double& n, double& result) -> void final { } // Do nothing

        std::string name;
    };

    template<>
    struct NodeTraits<NodeTypes::UnaryOperator> : INode {        
        virtual auto getType() const -> NodeTypes final { return NodeTypes::UnaryOperator; }
        virtual auto calculate(const double& n, double& result) -> void final;
        
        char operation;
        std::shared_ptr<INode> child; 
    };

    template<>
    struct NodeTraits<NodeTypes::Operator> : INode {
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Operator; }
        virtual auto calculate(const double& n, double& result) -> void final;        
        
        char operation;
        std::shared_ptr<INode> right; 
        std::shared_ptr<INode> left;
    };

    template<>
    struct NodeTraits<NodeTypes::Function> : INode {
        virtual auto getType() const -> NodeTypes final { return NodeTypes::Function; }
        virtual auto calculate(const double& n, double& result) -> void final;
        
        std::string name;
        std::shared_ptr<INode> argument;
    };

    inline static auto getNodeName(const kubvc::algorithm::NodeTypes& type) -> std::string {
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
            auto clear() -> void;
            auto createRoot() -> void;
        
            // Start validating ast from root node
            inline auto isValid() const -> bool { return isValidFrom(castToINodePtr<NodeTypes::Root>(m_root)); }

            // Start validating ast from specific node
            auto isValidFrom(std::shared_ptr<INode> start) const -> bool;
            auto createVariableNode(char value) -> NodePtr<NodeTypes::Variable>;
            auto createNumberNode(double value) -> NodePtr<NodeTypes::Number>;
            auto createOperatorNode(std::shared_ptr<INode> x,  std::shared_ptr<INode> y, char op) -> NodePtr<NodeTypes::Operator>;
            auto createUnaryOperatorNode(std::shared_ptr<INode> x, char op) -> NodePtr<NodeTypes::UnaryOperator>;
            auto createInvalidNode(std::string_view name) -> NodePtr<NodeTypes::Invalid>;
            auto createFunctionNode(std::string_view name) -> NodePtr<NodeTypes::Function>;

            inline auto getRoot() const -> NodePtr<NodeTypes::Root> { return m_root; }

        private:
            NodePtr<NodeTypes::Root> m_root;

            // Create node object 
            template <NodeTypes NodeType>
            auto createNode() const -> NodePtr<NodeType>;
    };
}

#include "ast_inline.h"