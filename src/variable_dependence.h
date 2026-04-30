#pragma once 
#include "singleton.h"
#include "ast_nodes.h"
#include "nodeTypes.h"
#include "logger.h"

#include <set>

namespace kubvc::math {
    enum class VariableSideDependence {
        Right,
        Left,
    };

    // Prefered variables in read mode 
    inline static constexpr std::initializer_list<char> PREFER_VARIABLES_REAL = {
        'x', 'y'
    };

    class VariableDependenceController {
        public:
            struct Variable {
                Variable() = default;
                explicit Variable(std::uint32_t _id, char _value, VariableSideDependence _side) : id(_id), value(_value), side(_side) { }

                std::uint32_t id = 0;
                char value = '\0';
                VariableSideDependence side = VariableSideDependence::Left;
            };

            VariableDependenceController() = default;
            ~VariableDependenceController();

            // Add new variable to controller 
            void add(VariableSideDependence side, std::uint32_t id, char value);
            void clear();

            [[nodiscard]] std::shared_ptr<Variable> getPreferedLeft() const { return m_preferedLeft; }
            [[nodiscard]] std::shared_ptr<Variable> getPreferedRight() const { return m_preferedRight; }

        private:
            std::shared_ptr<Variable> m_preferedLeft;
            std::shared_ptr<Variable> m_preferedRight;

            std::set<std::shared_ptr<Variable>> m_variables;
    };

    inline VariableDependenceController::~VariableDependenceController() {
        clear();
    }

    inline void VariableDependenceController::clear() {
        m_preferedLeft = nullptr;
        m_preferedRight = nullptr;
        m_variables.clear();
    }

    inline void VariableDependenceController::add(VariableSideDependence side, std::uint32_t id, char value) {
        const auto newVar = std::make_shared<Variable>(
            id,
            value,
            side);


        const bool isPrefered = std::find(PREFER_VARIABLES_REAL.begin(), 
                            PREFER_VARIABLES_REAL.end(), 
                            value) != PREFER_VARIABLES_REAL.end();
    
        if (isPrefered) {
            // Handle left-right side preferred variable
            if (side == VariableSideDependence::Left) {
                if (!m_preferedLeft) {
                    m_preferedLeft = newVar;
                }
            }
            else if (side == VariableSideDependence::Right) {
                if (!m_preferedRight) {
                    m_preferedRight = newVar;
                }
            }
        }

        m_variables.insert(newVar);
    }
}