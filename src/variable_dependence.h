#pragma once 
#include "ast_nodes.h"
#include "logger.h"

#include <optional>
#include <set>
#include <shared_mutex>

namespace kubvc::math {
    class VariableDependenceController {
        public:
            VariableDependenceController() = default;
            ~VariableDependenceController();

            enum class VariableSide {
                Right, 
                Left
            };

            struct Variable {
                static constexpr auto EMPTY_VALUE = '\0';

                char value = EMPTY_VALUE;
                VariableSide side = VariableSide::Left;
            };

            void set(VariableSide side, char value);
            void saveNodeAsParameter(algorithm::NodePtr<algorithm::NodeTypes::Variable> node);
            void reset();

            [[nodiscard]] std::optional<Variable> getVariableAtSide(VariableSide side) const;
            [[nodiscard]] std::span<const algorithm::NodePtr<algorithm::NodeTypes::Variable>> getParameterVariables() const;

        private:
            Variable m_left;
            Variable m_right;
            mutable std::shared_mutex m_mutex;
            std::vector<algorithm::NodePtr<algorithm::NodeTypes::Variable>> m_paramsVars;
    };

    inline VariableDependenceController::~VariableDependenceController() {
        reset();
    }

    using VDC = VariableDependenceController;
    
    inline std::span<const algorithm::NodePtr<algorithm::NodeTypes::Variable>> VDC::getParameterVariables() const {
        std::shared_lock lock(m_mutex);
        return m_paramsVars;
    }

    inline void VDC::saveNodeAsParameter(algorithm::NodePtr<algorithm::NodeTypes::Variable> node) {
        std::shared_lock lock(m_mutex);
        if (node->isParameter == true) {
            // Trying to find node with same value to avoid duplicates
            const auto& it = std::ranges::find_if(m_paramsVars, [node](auto findedNode) { 
                return findedNode->getValue() == node->getValue();
            });

            if (it == m_paramsVars.end()) {
                m_paramsVars.push_back(node);
            }
        }
    }

    inline void VDC::reset() {
        std::shared_lock lock(m_mutex);
        m_left.value = '\0';
        m_left.side = VDC::VariableSide::Left;

        m_right.value = '\0';
        m_right.side = VDC::VariableSide::Left;

        m_paramsVars.clear();
        m_paramsVars.shrink_to_fit();
    } 

    inline std::optional<VDC::Variable> VDC::getVariableAtSide(VDC::VariableSide side) const {
        std::shared_lock lock(m_mutex);
        switch (side) {
            case VDC::VariableSide::Left: {
                if (m_left.value != VDC::Variable::EMPTY_VALUE) {
                    return m_left;
                }
                break;
            }                
            case VDC::VariableSide::Right: {
                if (m_right.value != VDC::Variable::EMPTY_VALUE) {
                    return m_right;
                }
                break;
            } 
        } 
            
        return std::nullopt;    
    }

    inline void VDC::set(VDC::VariableSide side, char value) {
        // Handle left-right side variable
        std::shared_lock lock(m_mutex);
        switch (side) {
            case VDC::VariableSide::Left: {
                if (m_left.value != VDC::Variable::EMPTY_VALUE) {
                    return;
                }

                m_left.side = side;
                m_left.value = value;
                break;
            }        
            case VDC::VariableSide::Right: {
                if (m_right.value != VDC::Variable::EMPTY_VALUE) {
                    return;
                }

                m_right.side = side;
                m_right.value = value;
                break;
            }
        }      
    }

}