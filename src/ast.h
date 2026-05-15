#pragma once
#include "ast_nodes.h"

#include <shared_mutex>

namespace kubvc::algorithm {
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
            void setRoot(NodePtr<NodeTypes::Root> root);
            
        private:
            NodePtr<NodeTypes::Root> m_root;

            mutable std::shared_mutex m_mutex;
    };
}