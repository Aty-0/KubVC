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
                m_textBuffer(std::make_unique<ExpressionTextBuffer>()), 
                m_settings(std::make_unique<ExpressionVisualSettings>()),
                m_expression(std::make_unique<Expression>()), 
                m_id(id) {
                    m_settings->setRandomColor();
            }

            [[nodiscard]] ExpressionTextBuffer& getTextBuffer() const { return *m_textBuffer; }
            [[nodiscard]] Expression& getExpression() const { return *m_expression; }
            [[nodiscard]] ExpressionVisualSettings& getSettings() const { return *m_settings; }            
            [[nodiscard]] std::int32_t getId() const { return m_id; }

            // TODO: Not sure it supposed to be here 
            void parseThenEvaluate(const GraphLimits& limits); 
        
        private:
            std::int32_t m_id; 

            std::unique_ptr<ExpressionTextBuffer> m_textBuffer;
            std::unique_ptr<Expression> m_expression;
            std::unique_ptr<ExpressionVisualSettings> m_settings;
    };

    inline void ExpressionModel::parseThenEvaluate(const GraphLimits& limits) {
        static const auto taskManager = utility::TaskManager::getInstance();        
        static const auto builder = kubvc::algorithm::ASTBuilder::getInstance();

        taskManager->add([this, limits] {
            static const auto lexer = kubvc::algorithm::Lexer::getInstance();
            const auto result = lexer->tokenize(m_textBuffer->getBuffer().data());

            if (result.has_value()) {
                lexer->print(result.value());
                const auto buildResult = builder->build(m_expression->getTree(), m_expression->getVariableDependenceController(), result.value());
                m_expression->setValid(buildResult);
                m_expression->eval(limits);                
            } else {
                m_expression->setValid(false);
            }         
        });
    }
}