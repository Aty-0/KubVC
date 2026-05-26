#pragma once 
#include "nodeTypes.h"
#include <memory>
#include <string>
#include <cstdint>
#include <complex>

namespace kubvc::algorithm {
    template<NodeTypes NodeType> 
    struct NodeTraits { };

    struct INode {
        // Calcualate in real mode
        [[nodiscard]] virtual double calculate(double x, double y) = 0;
        
        // Calcualate in complex mode
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) = 0;

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
    inline static std::shared_ptr<INode> castToINodePtr(const NodePtr<Type>& ptr) { 
        return std::static_pointer_cast<INode>(ptr); 
    }


    template <NodeTypes Type>
    inline static NodePtr<Type> castToNodePtr(const std::shared_ptr<INode>& ptr) { 
        return std::static_pointer_cast<NodeTraits<Type>>(ptr); 
    }


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
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;

        std::shared_ptr<INode> child;
    };

    template<>
    struct NodeTraits<NodeTypes::Variable> : INode, NodeValue<char> { 
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Variable; }
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;

        float parameter = 0.0f;
        bool isParameter = false;
        bool useTimeForParameter = false;
    };

    template<>
    struct NodeTraits<NodeTypes::Number> : INode, NodeValue<double> {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Number; }
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
    };

    template<>
    struct NodeTraits<NodeTypes::Invalid> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Invalid; }
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
        std::string name;
    };

    template<>
    struct NodeTraits<NodeTypes::UnaryOperator> : INode {        
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::UnaryOperator; }
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
        
        char operation;
        std::shared_ptr<INode> child; 
    };

    template<>
    struct NodeTraits<NodeTypes::Operator> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Operator; }
        [[nodiscard]] virtual double calculate(double x, double y) final;        
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
        [[nodiscard]] std::complex<double> calculateComplexOperator(std::complex<double> leftNumber, std::complex<double> rightNumber);

        char operation;
        std::shared_ptr<INode> right; 
        std::shared_ptr<INode> left;
    };

    template<>
    struct NodeTraits<NodeTypes::ComplexNumber> : INode, NodeValue<std::complex<double>> {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::ComplexNumber; }
        [[nodiscard]] virtual double calculate(double x, double y) final;     
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
    };

    template<>
    struct NodeTraits<NodeTypes::Function> : INode {
        [[nodiscard]] virtual NodeTypes getType() const final { return NodeTypes::Function; }
        [[nodiscard]] virtual double calculate(double x, double y) final;
        [[nodiscard]] virtual std::complex<double> calculateComplex(double re, double im) final;
        
        std::string name;
        std::shared_ptr<INode> argument;
    };

    [[nodiscard]] inline static constexpr std::string_view getNodeName(kubvc::algorithm::NodeTypes type) {
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