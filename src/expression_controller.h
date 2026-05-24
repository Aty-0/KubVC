#pragma once 
#include "singleton.h"
#include "expression_model.h"
#include "logger.h"

#include <unordered_set>
#include <shared_mutex>
#include <span>

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
            
            // Run evaluate task for expression
            void evalExpression(std::shared_ptr<Expression> expr, const GraphLimits& limits);

            [[nodiscard]] std::shared_ptr<ExpressionModel> get(std::size_t index) const;
            [[nodiscard]] std::shared_ptr<ExpressionModel> getSelected() const { return m_selected; }

            [[nodiscard]] std::span<const std::shared_ptr<ExpressionModel>> getExpressions() const;
            [[nodiscard]] std::span<const std::shared_ptr<ExpressionModel>> getValidExpressions() const; 
            
            // Note: Use only for expressions. Because if parseThenEvaluate() fails 
            // or if clear() is called, your tasks will be destroyed 
            [[nodiscard]] utility::TaskManager& getTaskManager() { return m_taskManager; }

            [[nodiscard]] std::size_t getValidExpressionsSize() const { return m_validExpressions.size(); }
            [[nodiscard]] std::size_t getExpressionsSize() const { return m_expressions.size(); }

        private:
            utility::TaskManager m_taskManager;
            std::vector<std::shared_ptr<ExpressionModel>> m_validExpressions;
            std::vector<std::shared_ptr<ExpressionModel>> m_expressions;  
            std::shared_ptr<ExpressionModel> m_selected;
            mutable std::shared_mutex m_mutex;
    };

    inline void ExpressionController::parseThenEvaluate(std::shared_ptr<ExpressionModel> model, const GraphLimits& limits) {
        static const auto builder = kubvc::algorithm::ASTBuilder::getInstance();
        KUB_ASSERT(model != nullptr, "Model are nullptr");

        m_taskManager.add([this, model, limits] {
            static const auto lexer = kubvc::algorithm::Lexer::getInstance();
            const auto& expression = model->getExpression();
            const auto& textBuffer = model->getTextBuffer();            
            const auto result = lexer->tokenize(textBuffer->getBuffer().data());
            
            std::unique_lock lock(m_mutex);

            if (result.has_value()) {
                lexer->print(result.value());
                const auto buildResult = builder->build(expression->getTree(), expression->getVDC(), result.value());
                expression->setValid(buildResult, !buildResult ? "failed to build ast" : ""); // TODO: Reasons
                evalExpression(expression, limits);
                m_validExpressions.push_back(model);
            } else {
                // Remove model from list 
                std::erase(m_validExpressions, model);
                const auto lastError = lexer->getLastError();
                expression->setValid(false, lastError);
                
                lock.unlock();
            }         
        });
    }
    inline ExpressionController::~ExpressionController() {
        resetSelected();
        clear();
    }

    inline void ExpressionController::evalExpression(std::shared_ptr<Expression> expr, const GraphLimits& limits) {
        std::weak_ptr<Expression> weakExpression = expr;
        m_taskManager.add([weakExpression, limits]() {
            if (weakExpression.expired()) {
                return;
            }

            const auto lock = weakExpression.lock();
            lock->eval(limits);
        });
    }

    inline std::shared_ptr<ExpressionModel> ExpressionController::create() {
        std::unique_lock lock(m_mutex);
        static std::int32_t globalId = 0; 
        const auto model = std::make_shared<ExpressionModel>(globalId);
        m_expressions.push_back(model);
        globalId++;
        return model;
    } 

    inline void ExpressionController::clear() {
        m_taskManager.clear();
        //m_taskManager.waitForCompletion();

        {
            std::unique_lock lock(m_mutex);
            m_expressions.clear();
            m_expressions.shrink_to_fit();
            m_validExpressions.clear();
        }
    }
    
    inline void ExpressionController::resetSelected() {
        std::unique_lock lock(m_mutex);
        m_selected.reset();
        m_selected = nullptr;
    }

    inline bool ExpressionController::removeByIndex(std::size_t index) {
        std::unique_lock lock(m_mutex);
        if (index > m_expressions.size()) {
            return false;
        }

        const auto it = m_expressions.begin() + index;
        const auto model = *it;
        if (model) {
            std::erase(m_validExpressions, model);
        }  
        
        m_expressions.erase(it);
        return true;
    }

    inline bool ExpressionController::removeById(std::int32_t id) {
        std::unique_lock lock(m_mutex);
        const auto isModelWithId = [id](auto model) { return model && model->getId() == id; };
        const auto it = std::remove_if(m_expressions.begin(), m_expressions.end(), isModelWithId);

        if (it != m_expressions.end()) {
            std::erase_if(m_validExpressions, isModelWithId);
            m_expressions.erase(it);
            return true;
        } 
        
        return false;
    }

    inline std::shared_ptr<ExpressionModel> ExpressionController::get(std::size_t index) const {        
        std::unique_lock lock(m_mutex);
        return index < m_expressions.size() ? m_expressions[index] : nullptr;
    } 

    inline std::span<const std::shared_ptr<ExpressionModel>> ExpressionController::getExpressions() const {
        std::unique_lock lock(m_mutex);
        return m_expressions;
    }

    inline std::span<const std::shared_ptr<ExpressionModel>> ExpressionController::getValidExpressions() const {
        std::unique_lock lock(m_mutex);
        return m_validExpressions;
    }

}