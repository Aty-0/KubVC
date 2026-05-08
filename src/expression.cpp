#include "expression.h"
#include "logger.h"
#include "task_manager.h"


namespace kubvc::math {
    Expression::Expression()  : 
        m_tree(), 
        m_plotBuffer(std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE)),
        m_valid(false),
        m_lastErrorMessage() {
    
    }

    Expression::~Expression() {        
        m_valid = false;
        m_tree.clear();
        m_plotBuffer.clear();
        m_plotBuffer.shrink_to_fit();
    }

    void Expression::eval(const GraphLimits& limits, std::int32_t maxPointCount) {   
        if (!isValid())
            return; 

        static const auto taskManager = utility::TaskManager::getInstance();        
        taskManager->add([this, limits, maxPointCount] { 
            auto root = m_tree.getRoot();
            KUB_ASSERT(root != nullptr, "Root is nullptr, wtf");
            static const auto f = [](algorithm::NodePtr<algorithm::NodeTypes::Root> root, double x) {
                double out = 0.0;
                root->calculate(x, out);
                return out;
            };
            
            for (std::int32_t i = 0; i < maxPointCount; ++i) {
                const auto x0 = std::lerp(limits.xMin, limits.xMax, static_cast<double>(i) / (maxPointCount - 1));
                auto y0 = f(root, x0);
                m_plotBuffer[i] = { x0, y0 };
            }
        });        
    }

    void Expression::setValid(bool isValid, std::string lastMessage) {
        m_valid = isValid;
        if (!lastMessage.empty()) {
            m_lastErrorMessage = lastMessage;
        }
    }
}