#pragma once 
#include <glm/glm.hpp>
#include "ast.h"
#include "graph_limits.h"
#include "variable_dependence.h"
#include "primitives.h"

#include <mutex>
#include <shared_mutex>

namespace kubvc::math {
    class Expression {
        public:
            static constexpr auto MAX_PLOT_BUFFER_SIZE = 1024;
            static constexpr auto COMPLEX_GRID_SIZE = 32;
            static constexpr auto COMPLEX_GRID_LINES_COUNT = 128;

            Expression();
            Expression(const Expression& expression) = delete;
            Expression(Expression&& expression) = delete;

            ~Expression();
            
            // TODO: lock getters, use spans

            [[nodiscard]] math::VariableDependenceController& getVDC() { return m_vdc; }
            [[nodiscard]] algorithm::ASTree& getTree() { return m_tree; }
            [[nodiscard]] const std::vector<std::vector<glm::dvec2>>& getComplexGrid() const { return m_complexGrid; }
            [[nodiscard]] const std::vector<glm::dvec2>& getPlotBuffer() const { return m_plotBuffer; } 
            [[nodiscard]] const std::string& getLastErrorMessage() const { return m_lastErrorMessage; }
            [[nodiscard]] bool getRectMode() const;
            [[nodiscard]] bool isValid() const;
            [[nodiscard]] math::primitives::PrimitiveTypes getPrimitiveType() const;

            void setRectMode(bool rectMode);
            void setValid(bool isValid, std::string_view lastMessage);
            void setPrimitiveType(math::primitives::PrimitiveTypes type);

            template <primitives::IsPrimitive T>
            void setNewPrimitive(std::shared_ptr<T>&& primitive);

            template <primitives::IsPrimitive T>
            [[nodiscard]] std::shared_ptr<T> getPrimitive() const;

            // Evaluate current expression 
            void eval(const GraphLimits& limits, std::int32_t maxPointCount = MAX_PLOT_BUFFER_SIZE);
        private:
            math::VariableDependenceController m_vdc;
            // Abstract syntax tree for expressions 
            algorithm::ASTree m_tree;
            // Calculated points for graph
            std::vector<glm::dvec2> m_plotBuffer;  

            bool m_valid = false;
            std::string m_lastErrorMessage;

            mutable std::shared_mutex m_mutex;
            
            // Complex mode stuff:
            // TODO: Relocate 
            std::shared_ptr<primitives::IPrimitive> m_primitive;
            primitives::PrimitiveTypes m_primitiveType;
            bool m_rectMode;
            std::vector<std::vector<glm::dvec2>> m_complexGrid;
            void prepareComplexGrid();
    };

    template <primitives::IsPrimitive T>
    inline void Expression::setNewPrimitive(std::shared_ptr<T>&& primitive) {
        if (primitive) { 
            std::unique_lock lock(m_mutex);
            m_primitive = std::move(primitive);
        }
    }   
    
    template <primitives::IsPrimitive T>
    inline std::shared_ptr<T> Expression::getPrimitive() const {
        if (!m_primitive) {
            return nullptr;
        }

        std::shared_lock lock(m_mutex);
        return std::static_pointer_cast<T>(m_primitive);
    }
          
}