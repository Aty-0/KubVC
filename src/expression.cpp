#include "expression.h"
#include "logger.h"
#include <random>
#include "task_manager.h"

// TODO: Remove
#include "lexer.h"
#include "ast_builder.h"

namespace kubvc::math {
    std::vector<std::shared_ptr<Expression>> ExpressionController::Expressions = { };  
    std::shared_ptr<Expression> ExpressionController::Selected = nullptr;
        
    Expression::Expression()  : 
        m_id(-1), m_visible(true), 
        m_cursor(0), m_tree(), 
        m_valid(false),
        m_textBuffer(std::vector<char>(MAX_BUFFER_SIZE)),
        m_plotBuffer(std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE)),
        Settings({{1,1,1,1}, 1.0f, false, false, false}) {
        m_tree.createRoot();
    
        // Dummy id set
        static std::int32_t globalId = 0;
        globalId++;
        m_id = globalId;
    }

    Expression::~Expression() {
        KUB_DEBUG("Destroy expression id {} ...", m_id);
        
        m_visible = false;
        m_valid = false;
        m_tree.clear();
        m_id = -1;
        m_textBuffer.clear();
        m_textBuffer.shrink_to_fit();
        m_plotBuffer.clear();
        m_plotBuffer.shrink_to_fit();
    }

    void Expression::evalImpl(const GraphLimits& limits, std::int32_t maxPointCount) {
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
    }   
    
    void Expression::eval(const GraphLimits& limits, std::int32_t maxPointCount) {   
        static const auto taskManager = utility::TaskManager::getInstance();        
        if (!isValid())
            return;  
        taskManager->add([this, limits, maxPointCount] { 
            evalImpl(limits, maxPointCount);
        });        
    }

    void Expression::setRandomColor() {
        if (!Settings.isRandomColorSetted) {
            std::uniform_real_distribution<float> unif(0, 1.0f);
            std::random_device rd;
            std::default_random_engine re(rd());

            Settings.color = { unif(re), unif(re), unif(re), 1.0f };
            Settings.isRandomColorSetted = true;
        }
    }


    void Expression::parseThenEval(const GraphLimits& limits) {
        //static const auto parser = kubvc::algorithm::Parser::getInstance(); 
        static const auto taskManager = utility::TaskManager::getInstance();        

        static const auto lexer = kubvc::algorithm::Lexer::getInstance();
        static const auto builder = kubvc::algorithm::ASTBuilder::getInstance();

        taskManager->add([this, limits] {
            // TODO: Remove all parse; tree code 
            const auto result = lexer->tokenize(m_textBuffer.data());

            if (result.has_value()) {
                lexer->print(result.value());
                m_valid = builder->build(m_tree, result.value());
                if (m_valid) {
                    evalImpl(limits, MAX_PLOT_BUFFER_SIZE); 
                }
            } else {
                m_valid = false;
            }

            // TODO: Old, remove
            //parser->parse(m_tree, m_textBuffer.data());
            //m_valid = m_tree.isValid();                         
            //if (m_valid) {
            //    evalImpl(limits, MAX_PLOT_BUFFER_SIZE); 
            //}            
        }); 

        // Pre set random graph color  
        setRandomColor();           
    }
}