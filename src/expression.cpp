#include "expression.h"
#include "logger.h"
#include <random>
#include <future>

namespace kubvc::math {
    std::vector<std::shared_ptr<Expression>> ExpressionController::Expressions = { };  
    std::shared_ptr<Expression> ExpressionController::Selected = nullptr;
    
    Expression::Expression()  : 
        m_id(-1), m_visible(true), 
        m_cursor(0), m_tree(), 
        m_valid(false),
        m_workerStop(false),
        m_taskAvailable(false),
        m_textBuffer(std::vector<char>(MAX_BUFFER_SIZE)),
        m_plotBuffer(std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE)),
        Settings({{1,1,1,1}, 1.0f, false, false, false}) {
        m_tree.createRoot();
    
        // Dummy id set
        static std::int32_t globalId = 0;
        globalId++;
        m_id = globalId;
        
        m_workerThread = std::thread(&Expression::worker, this);
    }

    Expression::~Expression() {
        KUB_DEBUG("Destroy expression id {} ...", m_id);

        m_workerStop = true;        
        m_cv.notify_all();
        if(m_workerThread.joinable())
            m_workerThread.join();

        m_visible = false;
        m_valid = false;
        m_tree.clear();
        m_id = -1;
        m_textBuffer.clear();
        m_textBuffer.shrink_to_fit();
        m_plotBuffer.clear();
        m_plotBuffer.shrink_to_fit();
    }

    void Expression::worker() {
        while (!m_workerStop) {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_cv.wait(lk, [this]() { 
                return m_workerStop || m_taskAvailable;
            });

            if (m_workerStop)
                break;

            if (m_taskAvailable) {
                m_taskAvailable = false; 
                lk.unlock(); 
                evalImpl(m_currentEvalParams);        
            }            
        }
    }
            
    void Expression::evalImpl(const Expression::Params& params) {
        if (!isValid())
            return;     

        auto root = m_tree.getRoot();
        KUB_ASSERT(root != nullptr, "Root is nullptr, wtf");
        static const auto f = [](algorithm::NodePtr<algorithm::NodeTypes::Root> root, double x) {
            double out = 0.0;
            root->calculate(x, out);
            return out;
        };
        
        for (std::int32_t i = 0; i < params.maxPointCount; ++i) {
            const auto x0 = std::lerp(params.limits.xMin, params.limits.xMax, static_cast<double>(i) / (params.maxPointCount - 1));
            auto y0 = f(root, x0);
            std::lock_guard<std::mutex> lock(m_mutex); 
            m_plotBuffer[i] = { x0, y0 };
        }
    }   

    void Expression::eval(const GraphLimits& limits, std::int32_t maxPointCount) {          
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentEvalParams = { limits, maxPointCount};
        m_taskAvailable = true;
        m_cv.notify_one();

        // Pre set random graph color  
        if (!Settings.isRandomColorSetted) {
            std::uniform_real_distribution<float> unif(0, 1.0f);
            std::random_device rd;
            std::default_random_engine re(rd());

            Settings.color = { unif(re), unif(re), unif(re), 1.0f };
            Settings.isRandomColorSetted = true;
        }
    }

    void Expression::parseThenEval(const GraphLimits& limits) {
        static const auto parser = kubvc::algorithm::Parser::getInstance(); 
        parser->parse(m_tree, m_textBuffer.data());            
        m_valid = m_tree.isValid();
        eval(limits);    
    }
}