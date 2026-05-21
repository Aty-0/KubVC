#pragma once 
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <span>
#include <concepts>

namespace kubvc::math::primitives {
    enum class PrimitiveTypes {
        Circle,
        Rectangle
    };
    
    struct IPrimitive {
        IPrimitive() = default;
        ~IPrimitive() = default;

        virtual void generate(std::size_t pointsSize) = 0;

        [[nodiscard]] virtual PrimitiveTypes getType() = 0;
        [[nodiscard]] std::span<const glm::dvec2> getPoints() const;

        protected:
            mutable std::shared_mutex m_mutex;            
            std::vector<glm::dvec2> m_points;
    }; 

    inline std::span<const glm::dvec2> IPrimitive::getPoints() const {
        std::shared_lock lock { m_mutex };
        return m_points;
    }

    struct CirclePrimitive : public IPrimitive {
        CirclePrimitive(); 
        ~CirclePrimitive() = default;

        virtual PrimitiveTypes getType() final { return PrimitiveTypes::Circle; }
        virtual void generate(std::size_t pointsSize) final;

        glm::dvec2 center;
        double radius;
    };

    inline CirclePrimitive::CirclePrimitive() : center({0.0, 0.0}), radius(1.0) { }

    inline void CirclePrimitive::generate(std::size_t pointsSize) {
        std::unique_lock lock{ m_mutex };

        // If empty or size are different we are resize point buffer 
        if (m_points.size() != pointsSize) {
            m_points.resize(pointsSize);
        }

        constexpr auto TWO_PI = 2.0 * std::numbers::pi_v<double>;

        for (std::size_t i = 0; i < pointsSize; ++i) {
            const auto angle = (static_cast<double>(i) / pointsSize) * TWO_PI;
            const auto x = center.x + radius * std::cos(angle);
            const auto y = center.y + radius * std::sin(angle);
            
            m_points[i] = { x, y };
        }
    }

    struct RectanglePrimitive : public IPrimitive {
        RectanglePrimitive(); 
        ~RectanglePrimitive() = default;

        virtual PrimitiveTypes getType() final { return PrimitiveTypes::Rectangle; }
        virtual void generate(std::size_t pointsSize) final;

        glm::dvec4 rect;
    };

    inline RectanglePrimitive::RectanglePrimitive() : rect({1.0, 1.0, 1.0, 1.0}) { }

    inline void RectanglePrimitive::generate(std::size_t pointsSize) {
        std::unique_lock lock{ m_mutex };

        // If empty or size are different we are resize point buffer 
        const auto pointsPerSide = static_cast<std::int32_t>(pointsSize * 4); 
        if (m_points.size() != static_cast<std::size_t>(pointsPerSide)) {
            m_points.resize(pointsPerSide);
        }
        
        const auto halfSize = glm::vec2 { rect.z / 2, rect.w / 2 };

        for (std::int32_t i = 0; i < pointsPerSide; ++i) {
            const auto t = static_cast<double>(i) / (pointsPerSide - 1);
            m_points.push_back({ rect.x + std::lerp(-halfSize.x, halfSize.x, t), rect.y + halfSize.y });
        }

        for (std::int32_t i = 0; i < pointsPerSide; ++i) {
            const auto t = static_cast<double>(i) / (pointsPerSide - 1);
            m_points.push_back({ rect.x + halfSize.x, rect.y + std::lerp(halfSize.y, -halfSize.y, t) });
        }

        for (std::int32_t i = 0; i < pointsPerSide; ++i) {
            const auto t = static_cast<double>(i) / (pointsPerSide - 1);
            m_points.push_back({ rect.x + std::lerp(halfSize.x, -halfSize.x, t), rect.y - halfSize.y });
        }

        for (std::int32_t i = 0; i < pointsPerSide; ++i) {
            const auto t = static_cast<double>(i) / (pointsPerSide - 1);
            m_points.push_back({ rect.x - halfSize.x, rect.y + std::lerp(-halfSize.y, halfSize.y, t) });
        }
    }


    template<typename T> 
    concept IsPrimitive = std::derived_from<T, IPrimitive>;

    template<IsPrimitive T> 
    inline std::shared_ptr<T> makeNewPrimitive(std::size_t pointsSize) {                
        const auto primitive = std::make_shared<T>();
        primitive->generate(pointsSize);

        return primitive;
    }

}
