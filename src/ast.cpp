#include "ast.h"
#include "alg_helpers.h"
#include "logger.h"

namespace kubvc::algorithm
{
    Node::~Node()
    {
        //DEBUG("Node destructor");
    }
    
    ASTree::ASTree() 
    {

    }   

    void ASTree::clear()
    {
        m_root.reset();
    }

    void ASTree::makeRoot()
    {
        if (m_root != nullptr)
        {
            WARN("Root is already exist!");
            return;
        }

        m_root = createNode<RootNode>();
    }   
}