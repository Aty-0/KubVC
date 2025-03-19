#pragma once
#include <memory>
#include <string>
#include <functional>

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

        std::shared_ptr<Node> parent;

        // TODO: get set (set only if id is -1)
        std::int32_t id = -1; 
    };
    
    struct RootNode : Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Root; }

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
    };

    struct NumberNode : public NodeWithValue<double>
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Number; }
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
        
        char operation;
        std::shared_ptr<Node> right; 
        std::shared_ptr<Node> left;
    };

    // TODO: What's we need todo if we need to use something more complex 
    //       Like function with n arguments 

    struct FunctionNode : public Node
    {
        inline virtual auto getType() -> NodeTypes const final { return NodeTypes::Function; }
        
        std::string name;
        std::shared_ptr<Node> argument;
        // What's function we are represent 
        //std::function<double()> implementation;
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