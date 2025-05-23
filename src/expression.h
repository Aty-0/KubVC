#pragma once 
#include "renderer.h"
#include "ast.h"
#include "expression_parser.h"

// TODO: GraphLimits - save, read 

namespace kubvc::math
{
    class Expression
    {
        public:
            static constexpr auto MAX_FUNC_RANGE = 1024.0;
            static constexpr auto MAX_BUFFER_SIZE = 1024;
            static constexpr auto MAX_PLOT_BUFFER_SIZE = 2048;

            Expression() : 
                m_id(-1), m_visible(true), 
                m_cursor(0), m_tree(), 
                m_textBuffer(std::vector<char>(MAX_BUFFER_SIZE)),
                m_plotBuffer(std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE)),
                Settings({{1,1,1,1}, 1.0f, false, false, false})
            {
                m_tree.createRoot();

                // Dummy id set
                static std::int32_t globalId = 0;
                globalId++;

                m_id = globalId;
            }

            ~Expression();
            
            // Parse text buffer then evaluate
            inline void parseAndEval()
            {
                kubvc::algorithm::Parser::parse(m_tree, m_textBuffer.data());
                eval(MAX_FUNC_RANGE, -MAX_FUNC_RANGE, MAX_FUNC_RANGE, -MAX_FUNC_RANGE);    
                m_valid = m_tree.isValid();
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

            void eval(double xMax, double xMin, double yMax, double yMin, std::int32_t maxPointCount = MAX_PLOT_BUFFER_SIZE);
        private:
            void evalImpl(double xMax, double xMin, double yMax, double yMin, std::int32_t maxPointCount);
            
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
                                
            public:
                // Actually can be changed anywhere-anytime
                GraphSettings Settings;
    };

    namespace expressions
    {
        // TODO: Expression controller ?
        static std::vector<std::shared_ptr<Expression>> Expressions = { };  
        static std::shared_ptr<Expression> Selected = nullptr;
    }
}