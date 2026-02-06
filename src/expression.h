#pragma once 
#include "renderer.h"
#include "ast.h"
#include "expression_parser.h"
#include "graph_limits.h"

#include <thread>
#include <condition_variable>

namespace kubvc::math {
    class Expression {
        public:
            static constexpr auto MAX_FUNC_RANGE = 1024.0;
            static constexpr auto MAX_BUFFER_SIZE = 1024;
            static constexpr auto MAX_PLOT_BUFFER_SIZE = 1024;
            
            Expression();
            Expression(const Expression& expression) = delete;
            Expression(Expression&& expression) = delete;

            ~Expression();
            

            inline auto setVisible(bool visible) -> void { m_visible = visible; }
            inline auto isValid() -> bool const { return m_valid; }
            inline auto isVisible() -> bool const { return m_visible; }
            inline auto getCursor() -> std::int32_t const { return m_cursor; }
            inline auto getId() -> std::int32_t const { return m_id; }
            inline auto getPlotBuffer() -> std::vector<glm::dvec2> const { return m_plotBuffer; } 

            inline auto getTextBuffer() -> std::vector<char>& { return m_textBuffer; } 
            inline auto getTree() -> algorithm::ASTree& { return m_tree; }

            // Parse text buffer then evaluate
            auto parseThenEval(const GraphLimits& limits) -> void;
            // Evaluate current expression 
            auto eval(const GraphLimits& limits, std::int32_t maxPointCount = MAX_PLOT_BUFFER_SIZE) -> void;
        private:
            auto worker() -> void;

            struct EvalFuncImplParams {
                GraphLimits limits;
                std::int32_t maxPointCount;
            };

            using Params = EvalFuncImplParams;

            void evalImpl(const Params& params);

            Params m_currentEvalParams;

            // Show expression on graph 
            bool m_visible;
            // Is expression valid for ast
            bool m_valid;
            // Current expression id 
            std::int32_t m_id;
            // Current cursor position in text box 
            std::int32_t m_cursor;
            // Text buffer for text box   
            std::vector<char> m_textBuffer;  
            // Abstract syntax tree for expressions 
            kubvc::algorithm::ASTree m_tree;
            // Calculated points for graph
            std::vector<glm::dvec2> m_plotBuffer;  

            struct GraphSettings {
                glm::vec4 color;
                float thickness;
                bool shaded;
                bool changeColor;
                bool isRandomColorSetted;    
            };     

            std::thread m_workerThread;
            std::mutex m_mutex;
            std::condition_variable m_cv;
            std::atomic<bool> m_taskAvailable;
            std::atomic<bool> m_workerStop;

        public:
            // Actually can be changed anywhere-anytime
            GraphSettings Settings;

            inline auto setCursor(std::int32_t cursorPos) -> void {
                KUB_ASSERT(cursorPos >= 0, "Neg cursor pos");
                KUB_ASSERT(cursorPos < m_textBuffer.size(), "Cursor is bigger than text buffer size");
                m_cursor = cursorPos; 
            } 
    };
    
    class ExpressionController {
        public:
            static std::vector<std::shared_ptr<Expression>> Expressions;  
            static std::shared_ptr<Expression> Selected;
    };
}