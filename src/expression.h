#pragma once 
#include <glm/glm.hpp>
#include "ast.h"
#include "graph_limits.h"
    
namespace kubvc::math {
    class Expression {
        public:
            static constexpr auto MAX_PLOT_BUFFER_SIZE = 1024;
            
            Expression();
            Expression(const Expression& expression) = delete;
            Expression(Expression&& expression) = delete;

            ~Expression();
            
            [[nodiscard]] algorithm::ASTree& getTree() { return m_tree; }
            [[nodiscard]] const std::vector<glm::dvec2>& getPlotBuffer() const { return m_plotBuffer; } 
            [[nodiscard]] const std::string& getLastErrorMessage() const { return m_lastErrorMessage; }
            [[nodiscard]] bool isValid() const { return m_valid; }
            void setValid(bool isValid, std::string lastMessage);

            // Evaluate current expression 
            void eval(const GraphLimits& limits, std::int32_t maxPointCount = MAX_PLOT_BUFFER_SIZE);
        private:
            // Abstract syntax tree for expressions 
            kubvc::algorithm::ASTree m_tree;
            // Calculated points for graph
            std::vector<glm::dvec2> m_plotBuffer;  
            bool m_valid = false;
            std::string m_lastErrorMessage;
    };             
}