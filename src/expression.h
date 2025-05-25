#pragma once 
#include "renderer.h"
#include "ast.h"
#include "expression_parser.h"
#include "graph_limits.h"

#include <thread>
#include <condition_variable>

namespace kubvc::math
{
    class Expression
    {
        public:
            static constexpr auto MAX_FUNC_RANGE = 1024.0;
            static constexpr auto MAX_BUFFER_SIZE = 1024;
            static constexpr auto MAX_PLOT_BUFFER_SIZE = 2048;
            
            Expression();
            ~Expression();
            
            // Parse text buffer then evaluate
            inline void parseAndEval(const GraphLimits& limits)
            {
                kubvc::algorithm::Parser::parse(m_tree, m_textBuffer.data());            
                m_valid = m_tree.isValid();
                eval(limits);    
            }

            inline void setCursor(std::int32_t cursorPos) 
            {
                ASSERT(cursorPos >= 0, "Neg cursor pos");
                ASSERT(cursorPos < m_textBuffer.size(), "Cursor is bigger than text buffer size");

                m_cursor = cursorPos; 
            } 

            inline void setVisible(bool visible) { m_visible = visible; }
            
            inline bool isValid() const { return m_valid; }
            inline bool isVisible() const { return m_visible; }
            inline std::int32_t getCursor() const { return m_cursor; }
            inline std::int32_t getId() const { return m_id; }
            inline std::vector<glm::dvec2> getPlotBuffer() const { return m_plotBuffer; } 

            inline std::vector<char>& getTextBuffer() { return m_textBuffer; } 
            inline kubvc::algorithm::ASTree& getTree() { return m_tree; }

            void eval(const GraphLimits& limits, std::int32_t maxPointCount = MAX_PLOT_BUFFER_SIZE);
        private:
            void worker();

            struct EvalFuncImplParams
            {
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

            struct GraphSettings
            {
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
    };
    
    class ExpressionController
    {
        public:
            static std::vector<std::shared_ptr<Expression>> Expressions;  
            static std::shared_ptr<Expression> Selected;
    };
}