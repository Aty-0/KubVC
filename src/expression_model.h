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
            ExpressionModel(std::int32_t id) : m_id(id),
                m_textBuffer(std::make_shared<ExpressionTextBuffer>()), 
                m_expression(std::make_shared<Expression>()), 
                m_settings(std::make_shared<ExpressionVisualSettings>()) {
                    m_settings->setRandomColor();
            }

            [[nodiscard]] std::shared_ptr<ExpressionTextBuffer> getTextBuffer() const { return m_textBuffer; }
            [[nodiscard]] std::shared_ptr<Expression> getExpression() const { return m_expression; }
            [[nodiscard]] std::shared_ptr<ExpressionVisualSettings> getSettings() const { return m_settings; }            
            [[nodiscard]] std::int32_t getId() const { return m_id; }
                
        private:
            std::int32_t m_id; 

            std::shared_ptr<ExpressionTextBuffer> m_textBuffer;
            std::shared_ptr<Expression> m_expression;
            std::shared_ptr<ExpressionVisualSettings> m_settings;
    };
}