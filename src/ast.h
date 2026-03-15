#pragma once
#include "ast_nodes.h"

namespace kubvc::algorithm {
    template<NodeTypes NodeType>
    using NodePtr = std::shared_ptr<NodeTraits<NodeType>>;

    template <NodeTypes Type>
    inline static std::shared_ptr<INode> castToINodePtr(NodePtr<Type> ptr) { return std::static_pointer_cast<INode>(ptr); }

    template <NodeTypes Type>
    inline static NodePtr<Type> castToNodePtr(std::shared_ptr<INode> ptr) { return std::dynamic_pointer_cast<NodeTraits<Type>>(ptr); }

    class ASTree {
        public:
            ASTree() = default;                    
            ~ASTree();
            
            void clear();
            void clearFrom(std::shared_ptr<kubvc::algorithm::INode> start);

            [[nodiscard]] bool isRootExist() const { return m_root != nullptr; }
            // Start validating ast from root node
            [[nodiscard]] bool validate() const;
            // Start validating ast from specific node
            [[nodiscard]] bool validateFrom(std::shared_ptr<INode> start) const;
            [[nodiscard]] NodePtr<NodeTypes::Root> getRoot() const { return m_root; }
            void setRoot(NodePtr<NodeTypes::Root> root) { m_root = root; }
            
        private:
            NodePtr<NodeTypes::Root> m_root;
    };
}