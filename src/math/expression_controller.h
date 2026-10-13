#pragma once 
#include <unordered_set>
#include <shared_mutex>
#include <span>

#include "expression_model.h"

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
            void setSelected(std::shared_ptr<ExpressionModel> selected);
                       
            // Parse current text then evaluate 
            void parseThenEvaluate(std::shared_ptr<ExpressionModel> model, const GraphLimits& limits); 
            
            // Run evaluate task for expression
            void evalExpression(std::shared_ptr<Expression> expr, const GraphLimits& limits);
            
            void reevaluateAllExpressions(const GraphLimits& limits); 

            [[nodiscard]] std::shared_ptr<ExpressionModel> get(std::size_t index) const;
            [[nodiscard]] std::shared_ptr<ExpressionModel> getSelected() const;

            [[nodiscard]] std::vector<std::shared_ptr<ExpressionModel>> getExpressions() const;
            [[nodiscard]] std::vector<std::shared_ptr<ExpressionModel>> getValidExpressions() const; 
            
            [[nodiscard]] std::size_t getValidExpressionsSize() const;
            [[nodiscard]] std::size_t getExpressionsSize() const;

            // Note: Use only for expressions. Because if parseThenEvaluate() fails 
            // or if clear() is called, your tasks will be destroyed 
            [[nodiscard]] utility::TaskManager& getTaskManager() { return m_taskManager; }
        private:
            void callPlotRedraw();

            utility::TaskManager m_taskManager;
            std::vector<std::shared_ptr<ExpressionModel>> m_validExpressions;
            std::vector<std::shared_ptr<ExpressionModel>> m_expressions;  
            std::shared_ptr<ExpressionModel> m_selected;
            mutable std::shared_mutex m_mutex;
    };
}