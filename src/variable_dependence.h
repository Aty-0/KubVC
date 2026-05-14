#pragma once 
#include "ast_nodes.h"
#include "logger.h"

#include <optional>

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
            void reset();

            [[nodiscard]] std::optional<Variable> getVariableAtSide(VariableSide side) const;

        private:
            Variable m_left;
            Variable m_right;
    };

    inline VariableDependenceController::~VariableDependenceController() {
        reset();
    }

    using VDC = VariableDependenceController;
    inline void VDC::reset() {
        m_left.value = '\0';
        m_left.side = VDC::VariableSide::Left;

        m_right.value = '\0';
        m_right.side = VDC::VariableSide::Left;
    } 

    inline std::optional<VDC::Variable> VDC::getVariableAtSide(VDC::VariableSide side) const {
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