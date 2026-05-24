#pragma once
#include "ast_nodes.h"

#include <shared_mutex>
#include <stack>
#include <optional>
#include <atomic>

namespace kubvc::algorithm {

    struct TreeCacheView {
        TreeCacheView() : m_nodes(), m_cachePtr(nullptr) { } 
        explicit TreeCacheView(std::shared_ptr<std::vector<std::shared_ptr<INode>>> cache) : 
            m_nodes(cache ? *cache : std::span<const std::shared_ptr<INode>>{ }), 
            m_cachePtr(std::move(cache)) { }

        ~TreeCacheView() = default;

        [[nodiscard]] std::size_t size() const { return m_nodes.size(); }
        [[nodiscard]] bool empty() const { return m_nodes.empty(); }
        [[nodiscard]] auto begin() const { return m_nodes.begin(); }
        [[nodiscard]] auto end() const { return m_nodes.end(); }

        private:
            std::span<const std::shared_ptr<INode>> m_nodes; 
            std::shared_ptr<std::vector<std::shared_ptr<INode>>> m_cachePtr;
    };

    class ASTree {
        public:
            ASTree() = default;                    
            ~ASTree();
            
            void clear();

            [[nodiscard]] bool isRootExist() const;
            // Start validating ast from root node
            [[nodiscard]] bool validate();
            // Start validating ast from specific node
            [[nodiscard]] bool validateFrom(std::shared_ptr<INode> start) const;
            [[nodiscard]] NodePtr<NodeTypes::Root> getRoot() const;
            void setRoot(NodePtr<NodeTypes::Root> root);

            // Calcualate in real mode from root
            [[nodiscard]] double calculate(double x, double y); 

            // Calcualate in complex mode
            [[nodiscard]] std::complex<double> calculateComplex(double re, double im);
            
            // Get cached tree stack  
            [[nodiscard]] TreeCacheView getTreeCached() const; 
            
        private:
            [[nodiscard]] std::optional<std::stack<std::shared_ptr<INode>>> constructTreeStack(std::shared_ptr<INode> start) const;
            void makeTreeCache();

            // Current tree stack 
            std::stack<std::shared_ptr<INode>> m_treeStack;

            // We are use this construction for avoid condition race
            // also vector is pointer because some threads can use old cache
            // and it's a node cache using for calculations              
            std::atomic<std::shared_ptr<std::vector<std::shared_ptr<INode>>>> m_treeCached;
            std::atomic<NodePtr<NodeTypes::Root>> m_root;

            mutable std::shared_mutex m_mutex;
    };
}