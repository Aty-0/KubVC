#pragma once 
#include "nodeTypes.h"
#include <memory>
#include <string>
#include <cstdint>

namespace kubvc::algorithm {
    template<NodeTypes NodeType> 
    struct NodeTraits { };

    struct INode {
        virtual void calculate(double x, double y, double& result) = 0;
        
        [[nodiscard]] virtual NodeTypes getType() const = 0;
        [[nodiscard]] std::int32_t getId() const { return m_id; }
        void setId(std::uint32_t id) { if (m_id == DEFAULT_NODE_ID) { m_id = id; } }        

        private:
            static constexpr std::int8_t DEFAULT_NODE_ID = -1;
            std::int32_t m_id = DEFAULT_NODE_ID; 
    };

    template<NodeTypes NodeType>
    using NodePtr = std::shared_ptr<NodeTraits<NodeType>>;


    template <NodeTypes Type>
    inline static std::shared_ptr<INode> castToINodePtr(NodePtr<Type> ptr) { return std::static_pointer_cast<INode>(ptr); }


    template <NodeTypes Type>
    inline static NodePtr<Type> castToNodePtr(std::shared_ptr<INode> ptr) { return std::dynamic_pointer_cast<NodeTraits<Type>>(ptr); }


    template <typename ValueType>
    struct NodeValue {
        void setValue(ValueType value) { m_value = value; }
        [[nodiscard]] ValueType getValue() const { return m_value; }
        protected:        
            ValueType m_value;
    };

    
    template<>
    struct NodeTraits<NodeTypes::Root> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Root; }
        virtual void calculate(double x, double y, double& result) final;

        std::shared_ptr<INode> child;
    };

    template<>
    struct NodeTraits<NodeTypes::Variable> : INode, NodeValue<char> { 
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Variable; }
        virtual void calculate(double x, double y, double& result) final;

        float parameter = 0.0f;
        bool isParameter = false;
    };

    template<>
    struct NodeTraits<NodeTypes::Number> : INode, NodeValue<double> {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Number; }
        virtual void calculate([[maybe_unused]] double x, [[maybe_unused]] double y, double& result) final { result = m_value; }
    };

    template<>
    struct NodeTraits<NodeTypes::Invalid> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Invalid; }
        virtual void calculate([[maybe_unused]] double x, [[maybe_unused]] double y, [[maybe_unused]] double& result) final { } // Do nothing

        std::string name;
    };

    template<>
    struct NodeTraits<NodeTypes::UnaryOperator> : INode {        
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::UnaryOperator; }
        virtual void calculate(double x, double y, double& result) final;
        
        char operation;
        std::shared_ptr<INode> child; 
    };

    template<>
    struct NodeTraits<NodeTypes::Operator> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Operator; }
        virtual void calculate(double x, double y, double& result) final;        
        
        char operation;
        std::shared_ptr<INode> right; 
        std::shared_ptr<INode> left;
    };

    template<>
    struct NodeTraits<NodeTypes::ComplexNumber> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::ComplexNumber; }
        virtual void calculate([[maybe_unused]] double x, [[maybe_unused]] double y, [[maybe_unused]] double& result) final { }        
    };

    template<>
    struct NodeTraits<NodeTypes::Function> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Function; }
        virtual void calculate(double x, double y, double& result) final;
        
        std::string name;
        std::shared_ptr<INode> argument;
    };

    inline static constexpr std::string_view getNodeName(kubvc::algorithm::NodeTypes type) {
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
            default:            
                return "Unknown";
        }
    }
}

#include "ast_nodes_inline.h"