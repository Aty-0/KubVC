#pragma once
#include <memory>

#include "expression.h"
#include "expression_visual_settings.h"
#include "expression_text_buffer.h"

#include "task_manager.h"
#include "lexer.h"
#include "ast_builder.h"
#include "graph_limits.h"

namespace kubvc::math {
    class ExpressionModel {
        public:
            ExpressionModel(std::int32_t id) : 
                m_id(id),
                m_textBuffer(std::make_unique<ExpressionTextBuffer>()), 
                m_expression(std::make_unique<Expression>()), 
                m_settings(std::make_unique<ExpressionVisualSettings>())
            {
                    m_settings->setRandomColor();
            }

            [[nodiscard]] ExpressionTextBuffer& getTextBuffer() const { return *m_textBuffer; }
            [[nodiscard]] Expression& getExpression() const { return *m_expression; }
            [[nodiscard]] ExpressionVisualSettings& getSettings() const { return *m_settings; }            
            [[nodiscard]] std::int32_t getId() const { return m_id; }
                
        private:
            std::int32_t m_id; 

            std::unique_ptr<ExpressionTextBuffer> m_textBuffer;
            std::unique_ptr<Expression> m_expression;
            std::unique_ptr<ExpressionVisualSettings> m_settings;
    };
}