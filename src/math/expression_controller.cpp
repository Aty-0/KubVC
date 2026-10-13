#include "expression_controller.h"

#include "../algorithm/macro_controller.h"
#include "../utility/singleton.h"
#include "../utility/logger.h"

#include "../editor/editor.h"
#include "../editor/editor_plotter_window.h"

namespace kubvc::math {
    void ExpressionController::callPlotRedraw() {
        static auto editor = editor::Editor::getInstance();
        static auto plotterWindow = editor->get<editor::EditorPlotterWindow>();
        KUB_ASSERT(plotterWindow != nullptr, "Plotter window is nullptr");
        plotterWindow->redrawGraphs();
    }

    void ExpressionController::parseThenEvaluate(std::shared_ptr<ExpressionModel> model, const GraphLimits& limits) {
        static const auto builder = kubvc::algorithm::ASTBuilder::getInstance();
        KUB_ASSERT(model != nullptr, "Model are nullptr");

        m_taskManager.add([this, model, limits] {
            static const auto lexer = kubvc::algorithm::Lexer::getInstance();
            static const auto macroController = algorithm::MacroController::getInstance();
            
            const auto& expression = model->getExpression();
            const auto& textBuffer = model->getTextBuffer();
            auto text = std::string(textBuffer->getBuffer().data());
            // Replace all macro keywords
            macroController->appendMacrosToText(text);
            // Then we can tokenize
            const auto result = lexer->tokenize(text);
            
            if (result.has_value()) {
                lexer->print(result.value());
                const auto buildResult = builder->build(expression->getTree(), expression->getVDC(), result.value());
                expression->setValid(buildResult, !buildResult ? "failed to build ast" : ""); // TODO: Reasons
                evalExpression(expression, limits);
                
                std::unique_lock lock(m_mutex);
                m_validExpressions.push_back(model);
            } else {
                // Remove model from list 
                {
                    std::unique_lock lock(m_mutex);
                    std::erase(m_validExpressions, model);
                }
                const auto lastError = lexer->getLastError();
                expression->setValid(false, lastError);       
            }
            
            callPlotRedraw();         
        });
    }
    
    ExpressionController::~ExpressionController() {
        clear();
    }

    void ExpressionController::reevaluateAllExpressions(const GraphLimits& limits) {
        std::shared_lock lock(m_mutex);
        for (const auto& expr : m_validExpressions) {
            if (expr) {
                // Reset old results to avoid frozen graphs
                expr->getExpression()->resetBuffers();
                // Then trying to parse then evaluate expression 
                parseThenEvaluate(expr, limits);
            }
        }

        callPlotRedraw();
    }

    void ExpressionController::evalExpression(std::shared_ptr<Expression> expr, const GraphLimits& limits) {
        std::weak_ptr<Expression> weakExpression = expr;
        m_taskManager.add([weakExpression, limits]() {
            if (weakExpression.expired()) {
                return;
            }

            const auto lock = weakExpression.lock();
            lock->eval(limits);
        });
    }

    std::shared_ptr<ExpressionModel> ExpressionController::create() {
        std::unique_lock lock(m_mutex);
        static std::int32_t globalId = 0; 
        const auto model = std::make_shared<ExpressionModel>(globalId);
        m_expressions.push_back(model);
        globalId++;
        return model;
    } 

    void ExpressionController::clear() {
        m_taskManager.clear();
        {
            std::unique_lock lock(m_mutex);
            m_expressions.clear();
            m_expressions.shrink_to_fit();
            m_validExpressions.clear();
        }
        
        resetSelected();
    }
    
    void ExpressionController::resetSelected() {
        std::unique_lock lock(m_mutex);
        m_selected = nullptr;
    }

    bool ExpressionController::removeByIndex(std::size_t index) {
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

    bool ExpressionController::removeById(std::int32_t id) {
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

    std::shared_ptr<ExpressionModel> ExpressionController::get(std::size_t index) const {        
        std::shared_lock lock(m_mutex);
        return index < m_expressions.size() ? m_expressions[index] : nullptr;
    } 

    std::vector<std::shared_ptr<ExpressionModel>> ExpressionController::getExpressions() const {
        std::shared_lock lock(m_mutex);
        return m_expressions;
    }

    std::vector<std::shared_ptr<ExpressionModel>> ExpressionController::getValidExpressions() const {
        std::shared_lock lock(m_mutex);
        return m_validExpressions;
    }

    std::shared_ptr<ExpressionModel> ExpressionController::getSelected() const { 
        std::shared_lock lock(m_mutex);
        return m_selected; 
    }
    
    std::size_t ExpressionController::getValidExpressionsSize() const { 
        std::shared_lock lock(m_mutex);
        return m_validExpressions.size(); 
    }
    
    std::size_t ExpressionController::getExpressionsSize() const { 
        std::shared_lock lock(m_mutex);
        return m_expressions.size(); 
    }

    void ExpressionController::setSelected(std::shared_ptr<ExpressionModel> selected) {
        std::unique_lock lock(m_mutex);
        m_selected = std::move(selected); 
    }
}