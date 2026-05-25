#include "ast.h"
#include <mutex>

namespace kubvc::algorithm { 
    ASTree::~ASTree() {
        clear();
    }

    bool ASTree::isRootExist() const { 
        return m_root.load(std::memory_order_acquire) != nullptr;
    }

    NodePtr<NodeTypes::Root> ASTree::getRoot() const {
        return m_root.load(std::memory_order_acquire);
    }

    void ASTree::setRoot(NodePtr<NodeTypes::Root> root) { 
        m_root.store(std::move(root), std::memory_order_release); 
    }

    void ASTree::clear()  {        
        if (isRootExist()) {
            m_treeCached.store(nullptr, std::memory_order_seq_cst); 
            std::unique_lock lock(m_mutex);
            while(!m_treeStack.empty()) {
                auto node = m_treeStack.top();
                node = nullptr;
                m_treeStack.pop();
            } 
            lock.unlock();

            m_root.store(nullptr, std::memory_order_release);
        }
    }
    
    bool ASTree::validate() {
        if (!isRootExist()) {
            return false;
        } 
        
        // TODO: Relocate to build or something like that 
        auto stack = constructTreeStack(m_root.load(std::memory_order_acquire));
        if (!stack.has_value()) {
            return false;
        }

        std::unique_lock lock(m_mutex);
        m_treeStack = std::move(stack.value());
        m_treeCached.store(nullptr, std::memory_order_release); 
        return true;         
    }

    bool ASTree::validateFrom(std::shared_ptr<kubvc::algorithm::INode> start) const {        
        const auto stack = constructTreeStack(start); // Try to get stack, anyway we need check all tree 
        return stack.has_value();
    }

    std::optional<std::stack<std::shared_ptr<INode>>> ASTree::constructTreeStack(std::shared_ptr<INode> start) const {
        if (!start) {
            return std::nullopt;
        }
        std::stack<std::shared_ptr<INode>> tempStack;
        tempStack.push(start);

        std::stack<std::shared_ptr<INode>> stack;
        while (!tempStack.empty()) {
            auto nodeInterface = tempStack.top();
            stack.push(nodeInterface);
            tempStack.pop();

            switch (nodeInterface->getType()) {
                case kubvc::algorithm::NodeTypes::Operator: {
                    const auto node = castToNodePtr<NodeTypes::Operator>(nodeInterface); 
                    if (!node->left || !node->right) {
                        return std::nullopt;
                    }

                    tempStack.push(node->left);                 
                    tempStack.push(node->right);                 
                    break;    
                }
                case kubvc::algorithm::NodeTypes::UnaryOperator: {
                    const auto node = castToNodePtr<NodeTypes::UnaryOperator>(nodeInterface);
                    if (!node->child) {
                        return std::nullopt;
                    }

                    tempStack.push(node->child);         
                    break;     
                }
                case kubvc::algorithm::NodeTypes::Function: {
                    const auto node = castToNodePtr<NodeTypes::Function>(nodeInterface);
                    if (!node->argument) {
                        return std::nullopt;
                    }

                    tempStack.push(node->argument);         
                    break;
                }
                case kubvc::algorithm::NodeTypes::Root: {
                    const auto node = castToNodePtr<NodeTypes::Root>(nodeInterface);
                    if (!node->child) {
                        return std::nullopt;
                    }

                    tempStack.push(node->child);  
                    break;
                }
                // This nodes are doesn't have any childrens 
                case kubvc::algorithm::NodeTypes::Number:
                case kubvc::algorithm::NodeTypes::Variable:
                case kubvc::algorithm::NodeTypes::ComplexNumber:
                case kubvc::algorithm::NodeTypes::Invalid:                
                case kubvc::algorithm::NodeTypes::None:                
                    break;
                default: {
                    KUB_ERROR("Unknown node type {}", static_cast<std::int32_t>(nodeInterface->getType()));
                    return std::nullopt;
                }
            }
        }
        
        return stack;
    }
    
    void ASTree::makeTreeCache() {
        KUB_DEBUG("ASTree::makeTreeCache");
        if (m_treeStack.empty()) {
            m_treeCached.store(nullptr, std::memory_order_release);
            return;
        }
        auto newCache = std::make_shared<std::vector<std::shared_ptr<INode>>>();
        auto stack = m_treeStack;
        newCache->reserve(stack.size());
        while (!stack.empty()) {
            newCache->push_back(stack.top());
            stack.pop();
        }

#ifdef KUB_AST_CACHE_PRINT_DEBUG
        KUB_DEBUG("Cache size: {}", newCache->size());
        for (std::size_t i = 0; i < newCache->size(); i++) {
            KUB_DEBUG("[{}] type={}", i, getNodeName((*newCache)[i]->getType()));
        }
#endif

        m_treeCached.store(std::move(newCache), std::memory_order_release);
    }
    
    constexpr std::size_t VALUE_STACK_SIZE = 512;

    double ASTree::calculate(double x, double y) {
        if (!isRootExist()) {
            return std::numeric_limits<double>::quiet_NaN();
        }

        auto cached = m_treeCached.load(std::memory_order_acquire);
        if (!cached) {
            std::unique_lock lock { m_mutex }; 
            // Check again with lock
            cached = m_treeCached.load(std::memory_order_relaxed);
            if (!cached) {
                makeTreeCache();
                cached = m_treeCached.load(std::memory_order_relaxed);
            }
        }
   
        KUB_ASSERT(cached->size() < VALUE_STACK_SIZE, "ast: cached size > value stack size");

        if (!cached || cached->empty()) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        thread_local static double valueStack[VALUE_STACK_SIZE];
        std::size_t top = 0;
        for (const auto& node : (*cached)) {            
            switch (node->getType()) {
                case kubvc::algorithm::NodeTypes::Operator: {
                    if (top < 2) {
                        KUB_ERROR("Stack underflow at Operator node");
                        return std::numeric_limits<double>::quiet_NaN();
                    }

                    auto right = valueStack[--top]; 
                    auto left = valueStack[--top]; 
                    valueStack[top++] = node->calculate(left, right);
                    break;    
                }
                case kubvc::algorithm::NodeTypes::Root: 
                case kubvc::algorithm::NodeTypes::Function:
                case kubvc::algorithm::NodeTypes::UnaryOperator: {
                    if (top == 0) {
                        KUB_ERROR("Stack underflow at unary node");
                        return std::numeric_limits<double>::quiet_NaN();
                    }

                    auto operand = valueStack[--top]; 
                    valueStack[top++] = node->calculate(operand, 0.0);
                    break;     
                }
                case kubvc::algorithm::NodeTypes::Number:
                case kubvc::algorithm::NodeTypes::Variable:
                case kubvc::algorithm::NodeTypes::ComplexNumber:
                    valueStack[top++] = node->calculate(x, y);
                    break;
                case kubvc::algorithm::NodeTypes::None:
                case kubvc::algorithm::NodeTypes::Invalid:
                    break;  
            }
        }
        
        if (top == 0) 
            return std::numeric_limits<double>::quiet_NaN();

        return valueStack[--top]; 
    }            
    
    std::complex<double> ASTree::calculateComplex(double re, double im) {
        if (!isRootExist()) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
        }

        auto cached = m_treeCached.load(std::memory_order_acquire);
        if (!cached) {
            std::unique_lock lock { m_mutex }; 
            // Check again with lock
            cached = m_treeCached.load(std::memory_order_relaxed);
            if (!cached) {
                makeTreeCache();
                cached = m_treeCached.load(std::memory_order_relaxed);
            }
        }

        KUB_ASSERT(cached->size() < VALUE_STACK_SIZE, "ast: cached size > value stack size");
        
        if (!cached || cached->empty()) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
        }

        thread_local static std::complex<double> valueStack[VALUE_STACK_SIZE];
        std::size_t top = 0;
        for (auto& node : (*cached)) {  
            switch (node->getType()) {
                case kubvc::algorithm::NodeTypes::Operator: {       
                    if (top < 2) {
                        KUB_ERROR("Stack underflow at Operator node");
                        return std::numeric_limits<double>::quiet_NaN();
                    }

                    // Get left/right node for operators
                    auto right = valueStack[--top];                     
                    auto left = valueStack[--top]; 

                    // Calculate operator result 
                    auto operatorNode = castToNodePtr<NodeTypes::Operator>(node); 
                    valueStack[top++] = operatorNode->calculateComplexOperator(left, right);
                    break;    
                }
                case kubvc::algorithm::NodeTypes::UnaryOperator:
                case kubvc::algorithm::NodeTypes::Root: 
                case kubvc::algorithm::NodeTypes::Function: {
                    if (top == 0) {
                        KUB_ERROR("Stack underflow at unary node");
                        return std::numeric_limits<double>::quiet_NaN();
                    }

                    auto operand = valueStack[--top]; 
                    valueStack[top++] = node->calculateComplex(operand.real(), operand.imag());
                    break;     
                }
                // This nodes are doesn't have any childrens 
                case kubvc::algorithm::NodeTypes::Number:
                case kubvc::algorithm::NodeTypes::Variable:
                case kubvc::algorithm::NodeTypes::ComplexNumber:
                    valueStack[top++] = node->calculateComplex(re, im);
                    break;
                case kubvc::algorithm::NodeTypes::Invalid: 
                case kubvc::algorithm::NodeTypes::None: 
                    break;               
                default: {
                    KUB_ASSERT(false, "Unknown node type {}", static_cast<std::int32_t>(node->getType()));
                    break;
                }
            }        
        } 

        if (top == 0) {
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
        }

        return valueStack[--top];
    }

    TreeCacheView ASTree::getTreeCached() const {
        auto cached = m_treeCached.load(std::memory_order_acquire);
        if (!cached) {
            std::unique_lock lock { m_mutex }; 
            // Check again with lock
            cached = m_treeCached.load(std::memory_order_relaxed);    
        }

        return TreeCacheView { cached };
    }
}