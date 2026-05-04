#pragma once 
#include "singleton.h"
#include "expression_model.h"

namespace kubvc::math {
    class ExpressionController : public utility::Singleton<ExpressionController> {
        public:
            ExpressionController() = default;
            ~ExpressionController();

            [[nodiscard]] std::shared_ptr<ExpressionModel> create();
            void clear();
            bool removeById(std::int32_t index);
            bool removeByIndex(std::size_t index);
            void resetSelected();
            void setSelected(std::shared_ptr<ExpressionModel> selected) { m_selected = std::move(selected); }
            
            [[nodiscard]] std::shared_ptr<ExpressionModel> get(std::size_t index) const;
            [[nodiscard]] std::shared_ptr<ExpressionModel> getSelected() const { return m_selected; }
            [[nodiscard]] std::vector<std::shared_ptr<ExpressionModel>> getExpressions() const { return m_expressions; }

        private:
            std::vector<std::shared_ptr<ExpressionModel>> m_expressions;  
            std::shared_ptr<ExpressionModel> m_selected;
    };

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
    }
    
    inline void ExpressionController::resetSelected() {
        m_selected.reset();
        m_selected = nullptr;
    }

    inline bool ExpressionController::removeByIndex(std::size_t index) {
        const auto it = m_expressions.erase(m_expressions.begin() + index);
        return it == m_expressions.end();
    }

    inline bool ExpressionController::removeById(std::int32_t id) {
        const auto it = m_expressions.erase(std::remove_if(m_expressions.begin(), m_expressions.end(), 
            [id](auto model) {
                if (model == nullptr)
                    return false;

                return model->getId() == id; 
        }));

        return it == m_expressions.end();
    }

    inline std::shared_ptr<ExpressionModel> ExpressionController::get(std::size_t index) const {
        if (index >= m_expressions.size()) {
            return nullptr;
        }

        return m_expressions.at(index);
    } 
}