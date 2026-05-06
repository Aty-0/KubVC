#pragma once 
#include "singleton.h"
#include "expression_model.h"
#include "logger.h"

#include <unordered_set>

namespace kubvc::math {
    class ExpressionController : public utility::Singleton<ExpressionController> {
        public:
            ExpressionController() = default;
            ~ExpressionController();

            std::shared_ptr<ExpressionModel> create();
            void clear();
            bool removeById(std::int32_t index);
            bool removeByIndex(std::size_t index);
            void resetSelected();
            void setSelected(std::shared_ptr<ExpressionModel> selected) { m_selected = std::move(selected); }
            void parseThenEvaluate(std::shared_ptr<ExpressionModel> model, const GraphLimits& limits); 
        
            [[nodiscard]] std::shared_ptr<ExpressionModel> get(std::size_t index) const;
            [[nodiscard]] std::shared_ptr<ExpressionModel> getSelected() const { return m_selected; }
            [[nodiscard]] std::vector<std::shared_ptr<ExpressionModel>> getExpressions() const { return m_expressions; }
            [[nodiscard]] std::unordered_set<std::shared_ptr<ExpressionModel>> getValidExpressions() const { return m_validExpressions; } 
        private:
            std::unordered_set<std::shared_ptr<ExpressionModel>> m_validExpressions;
            std::vector<std::shared_ptr<ExpressionModel>> m_expressions;  
            std::shared_ptr<ExpressionModel> m_selected;
    };

    inline void ExpressionController::parseThenEvaluate(std::shared_ptr<ExpressionModel> model, const GraphLimits& limits) {
        static const auto taskManager = utility::TaskManager::getInstance();        
        static const auto builder = kubvc::algorithm::ASTBuilder::getInstance();
        KUB_ASSERT(model != nullptr, "Model are nullptr");

        taskManager->add([this, model, limits] {
            static const auto lexer = kubvc::algorithm::Lexer::getInstance();
            auto& expression = model->getExpression();
            auto& textBuffer = model->getTextBuffer();            
            const auto result = lexer->tokenize(textBuffer.getBuffer().data());
            if (result.has_value()) {
                lexer->print(result.value());
                const auto buildResult = builder->build(expression.getTree(), result.value());
                expression.setValid(buildResult);
                expression.eval(limits);              
                m_validExpressions.insert(model);
            } else {
                // Remove model from list 
                if (m_validExpressions.contains(model)) {
                    m_validExpressions.erase(model);
                }                
                expression.setValid(false);
            }         
        });
    }

    inline ExpressionController::~ExpressionController() {
        clear();
    }

    inline std::shared_ptr<ExpressionModel> ExpressionController::create() {
        static std::int32_t globalId = 0; 
        const auto model = std::make_shared<ExpressionModel>(globalId);
        m_expressions.push_back(model);
        globalId++;
        return model;
    } 

    inline void ExpressionController::clear() {
        m_expressions.clear();
        m_expressions.shrink_to_fit();

        m_validExpressions.clear();
    }
    
    inline void ExpressionController::resetSelected() {
        m_selected.reset();
        m_selected = nullptr;
    }

    inline bool ExpressionController::removeByIndex(std::size_t index) {
        const auto it = m_expressions.begin() + index;
        const auto model = *it;
        if (m_validExpressions.contains(model)) {
            m_validExpressions.erase(model);
        }  
        
        const auto eraseIt = m_expressions.erase(it);
        return eraseIt == m_expressions.end();
    }

    inline bool ExpressionController::removeById(std::int32_t id) {
        const auto findModelById = [id](auto model) {
            if (model == nullptr)
                return false;

            return model->getId() == id; 
        };

        const auto findIt = std::find_if(m_expressions.begin(), m_expressions.begin(), findModelById);
        if (findIt != m_expressions.end()) {
            const auto model = *findIt;
            if (m_validExpressions.contains(model)) {
                m_validExpressions.erase(model);
            }  
        }
        
        const auto it = m_expressions.erase(std::remove_if(m_expressions.begin(), m_expressions.end(), findModelById));
        return it == m_expressions.end();
    }

    inline std::shared_ptr<ExpressionModel> ExpressionController::get(std::size_t index) const {
        if (index >= m_expressions.size()) {
            return nullptr;
        }

        return m_expressions.at(index);
    } 
}