#pragma once 
#include <glm/glm.hpp>
#include <random>

namespace kubvc::math {
    struct ExpressionVisualSettings {
        ExpressionVisualSettings() = default;
                              
        void setShaded(bool shaded) { m_shaded = shaded; }
        void setVisible(bool visible) { m_visible = visible; }
        void setExpandTextBox(bool expand) { m_expandTextBox = expand; }
        void setThickness(float thickness) { m_thickness = thickness; }
        void setColor(const glm::vec4& color) { m_color = color; }
        void setChangeColor(bool changeColor) { m_changeColor = changeColor; }
        void setRandomColor();

        [[nodiscard]] bool getShaded() const { return m_shaded; }
        [[nodiscard]] bool getVisible() const { return m_visible; }
        [[nodiscard]] bool getChangeColor() const { return m_changeColor; }
        [[nodiscard]] float getThickness() const { return m_thickness; }
        [[nodiscard]] bool getExpandTextBox() const { return m_expandTextBox; }
        [[nodiscard]] glm::vec4 getColor() const { return m_color; }

        private:
            glm::vec4 m_color = glm::vec4(1,1,1,1);
            float m_thickness = 1.0f;
            bool m_expandTextBox = false;
            bool m_shaded = false;
            bool m_changeColor = false;
            bool m_visible = true;
            bool m_isRandomColorSetted = false;  
    };

    inline void ExpressionVisualSettings::setRandomColor() {
        if (!m_isRandomColorSetted) {
            std::uniform_real_distribution<float> unif(0, 1.0f);
            std::random_device rd;
            std::default_random_engine re(rd());
            m_color = { unif(re), unif(re), unif(re), 1.0f };
            m_isRandomColorSetted = true;
        }
    }
}