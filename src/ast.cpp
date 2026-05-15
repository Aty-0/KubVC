#include "ast.h"
#include <mutex>

namespace kubvc::algorithm { 
    ASTree::~ASTree() {
        clear();
    }
    
    void ASTree::setRoot(NodePtr<NodeTypes::Root> root) { 
        std::unique_lock lock(m_mutex);
        m_root = root; 
    }

    void ASTree::clear()  {        
        if (isRootExist()) {
            std::unique_lock lock(m_mutex);

            clearFrom(m_root);
            m_root = nullptr;
        }/* else {
            KUB_DEBUG("Can't clear ast because root is nullptr");
        }*/
    }

    void ASTree::clearFrom(std::shared_ptr<kubvc::algorithm::INode> start) {
        if (start == nullptr) {
            return;
        }

        const auto type = start->getType();
        switch (type) {
            case kubvc::algorithm::NodeTypes::Root: {
                const auto node = castToNodePtr<NodeTypes::Root>(start);
                clearFrom(node->child);
                node->child = nullptr;
                break;
            }
            case kubvc::algorithm::NodeTypes::Operator: {
                const auto node = castToNodePtr<NodeTypes::Operator>(start);                  
                // Recursively clear left and right subtrees
                if (node->left != nullptr) {
                    clearFrom(node->left);
                    node->left = nullptr;
                }

                if (node->right != nullptr) {
                    clearFrom(node->right);
                    node->right = nullptr;
                }
                break;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                const auto node = castToNodePtr<NodeTypes::UnaryOperator>(start);         
                if (node->child != nullptr) {
                    clearFrom(node->child);
                    node->child = nullptr;
                }
                break;     
            }
            case kubvc::algorithm::NodeTypes::Function: {
                const auto node = castToNodePtr<NodeTypes::Function>(start);         
                if (node->argument != nullptr) {
                    clearFrom(node->argument);
                    node->argument = nullptr;
                }
                break;
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
            case kubvc::algorithm::NodeTypes::Invalid:
                // Nothing to clear 
                break;
            default:
                KUB_ASSERT(true, "Unknown type");
                break;
        }
    }
    
    bool ASTree::validate() const {
        std::unique_lock lock(m_mutex);

        if (!isRootExist()) {
            return false;
        } 

        return validateFrom(castToINodePtr<NodeTypes::Root>(m_root)); 
    }

    bool ASTree::validateFrom(std::shared_ptr<kubvc::algorithm::INode> start) const {
        if (start == nullptr) {
            return false;
        }

        const auto type = start->getType();
        switch (type) {
            case kubvc::algorithm::NodeTypes::Root: {
                const auto node = castToNodePtr<NodeTypes::Root>(start);
                return validateFrom(node->child);
            }
            case kubvc::algorithm::NodeTypes::Number:
            case kubvc::algorithm::NodeTypes::Variable:
                return true;            
            case kubvc::algorithm::NodeTypes::Operator: {
                const auto node = castToNodePtr<NodeTypes::Operator>(start);         
                bool resultLeft = false;
                bool resultRight = false;

                if (node->left != nullptr) {
                    resultLeft = validateFrom(node->left);
                }

                if (node->right != nullptr) {
                    resultRight = validateFrom(node->right);
                }

                return resultLeft && resultRight;    
            }
            case kubvc::algorithm::NodeTypes::UnaryOperator: {
                const auto node = castToNodePtr<NodeTypes::UnaryOperator>(start);         
                return validateFrom(node->child);     
            }
            case kubvc::algorithm::NodeTypes::Function: {
                const auto node = castToNodePtr<NodeTypes::Function>(start);         
                if (node->argument != nullptr) {
                    return validateFrom(node->argument);     
                }
                else {
                    return false;
                }
            }
            default:
                return false;
        }

        return false;
    }
}