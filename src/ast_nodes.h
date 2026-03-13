#pragma once 
#include "nodeTypes.h"
#include <memory>
#include <string>
#include <cstdint>

namespace kubvc::algorithm {
 struct INode {
        virtual NodeTypes getType() const = 0;
        virtual void calculate(const double& n, double& result) = 0;
        
        inline std::int32_t getId() const { return m_id; }
        inline void setId(std::uint32_t id) { if (m_id == DEFAULT_NODE_ID) { m_id = id; } }        

        private:
            static constexpr std::uint8_t DEFAULT_NODE_ID = -1;
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
        virtual void calculate(const double& n, double& result) final;

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
}

#include "ast_nodes_inline.h"