#include "expression.h"
#include "logger.h"
#include "expression_controller.h"
#include "application_config.h"

namespace kubvc::math {
    Expression::Expression()  : 
        m_tree(), 
        m_plotBuffer(std::make_shared<std::vector<glm::dvec2>>(MAX_PLOT_BUFFER_SIZE)),
        m_valid(false),
        m_lastErrorMessage(),
        m_primitiveType(primitives::PrimitiveTypes::Circle),
        m_rectMode(false),
        m_complexGrid(std::make_shared<std::vector<std::vector<glm::dvec2>>>(COMPLEX_GRID_SIZE, std::vector<glm::dvec2>(COMPLEX_GRID_LINES_COUNT))) {
            // Set default primitive
            setNewPrimitive(primitives::makeNewPrimitive<primitives::CirclePrimitive>(MAX_PLOT_BUFFER_SIZE));
    }

    Expression::~Expression() {    
        std::unique_lock lock(m_mutex); 
        m_valid = false;
        m_vdc.reset();
        m_tree.clear();
    }

    static constexpr std::uint8_t NEWTON_MAX_ITER = 16; 
    static constexpr auto EPS = 1e-10;

    inline static double solveBisection(std::function<double(double)> f, double min, double max) {
        auto fMin = f(min);
        auto fMax = f(max);
        
        if (fMin * fMax >= 0.0) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        while (max - min > 1e-12) {
            const auto mid = (max + min) * 0.5;
            const auto fMid = f(mid);
            
            if (fMid == 0.0) {
                return mid;
            }
            
            if (fMin * fMid < 0.0) {
                max = mid;
                fMax = fMid;
            } else {
                min = mid;
                fMin = fMid;
            }
        }
        
        return (max + min) * 0.5;
    }

    inline static double solveNewton(std::function<double(double)> f, double min, double max) {
        const auto diff = max - min;
        const auto eps_step = 1e-7 * diff;
        const auto eps_abs  = 1e-10 * diff;

        double x0 = (min + max) * 0.5;
        double fx0  = f(x0);
        if (glm::isnan(fx0)) {
            return std::numeric_limits<double>::quiet_NaN();
        } 

        for (std::int32_t i = 0; i < NEWTON_MAX_ITER; ++i) {
            const double dfx0 = (f(x0 + EPS) - f(x0 - EPS)) / (2.0 * EPS);
            if (glm::isnan(dfx0) || dfx0 == 0.0)
                return std::numeric_limits<double>::quiet_NaN();

            const double delta = fx0 / dfx0;
            x0 -= delta;

  
            if (x0 < min || x0 > max) {
                return solveBisection(f, min, max); 
            }

            fx0 = f(x0);
            if (glm::isnan(fx0)) {
                return std::numeric_limits<double>::quiet_NaN();
            } 

            if (glm::abs(delta) < eps_step && glm::abs(fx0) < eps_abs)
                return x0;
        }
        return std::numeric_limits<double>::quiet_NaN();
    }

    void Expression::eval(const GraphLimits& limits) {   
        if (!isValid()) {
            return; 
        }

        static const auto appConfig = application::ApplicationConfig::getInstance();        
        switch (appConfig->getMode()) {
            case application::MathMode::Complex: {
                if (m_rectMode) {
                    const auto& front = m_complexGrid.front();
                    for (std::size_t i = 0; i < COMPLEX_GRID_SIZE; ++i) {
                        const auto x = std::lerp(limits.xMin, limits.xMax, static_cast<double>(i) / (COMPLEX_GRID_SIZE - 1));
                        for (std::size_t j = 0; j < COMPLEX_GRID_LINES_COUNT; ++j) {
                            const auto y = std::lerp(limits.yMin, limits.yMax, static_cast<double>(j) / (COMPLEX_GRID_LINES_COUNT - 1));
                            const auto w = m_tree.calculateComplex(x, y);                        
                            (*front)[i][j] = { w.real(), w.imag() };
                        }
                    }
                    m_complexGrid.swap();                    
                } else {                                            
                    if (!m_primitive) {
                        return;
                    }

                    const auto& front = m_plotBuffer.front();
                    const auto points = m_primitive->getPoints();
                    for (std::size_t i = 0; i < points.size(); ++i) {                            
                        const auto point = points[i];
                        const auto w = m_tree.calculateComplex(point.x, point.y);
                        
                        (*front)[i] = { w.real(), w.imag() };
                    }

                    m_plotBuffer.swap();
                }                    
                break;        
            }
            case application::MathMode::Real: {
                const auto left = m_vdc.getVariableAtSide(math::VDC::VariableSide::Left);
                const bool isYPrefered = !left.has_value() || left.value().value == 'y';
                const auto& front = m_plotBuffer.front();
                for (std::int32_t i = 0; i < MAX_PLOT_BUFFER_SIZE; ++i) {                              
                    if (isYPrefered) {                    
                        const auto x0 = std::lerp(limits.xMin, limits.xMax, static_cast<double>(i) / (MAX_PLOT_BUFFER_SIZE - 1));
                        const auto f = [this, x0](const double y) {
                            return m_tree.calculate(x0, y) - y;
                        };  
                        const auto y0 = solveNewton(f, limits.yMin, limits.yMax);
                        (*front)[i] = { x0, y0 };
                    } else {
                        const auto y0 = std::lerp(limits.yMin, limits.yMax, static_cast<double>(i) / (MAX_PLOT_BUFFER_SIZE - 1));
                        const auto f = [this, y0](const double x) {
                            return m_tree.calculate(x, y0) - x;
                        };  
                        const auto x0 = solveNewton(f, limits.xMin, limits.xMax);
                        (*front)[i] = { x0, y0 };
                    }
                } 
                m_plotBuffer.swap();
                break;        
            }
        }
    }

    void Expression::setValid(bool isValid, std::string_view lastMessage) {
        std::unique_lock lock(m_mutex);        
        m_valid = isValid;
        if (!lastMessage.empty()) {
            m_lastErrorMessage = lastMessage;
        }
    }
    
    bool Expression::isValid() const {
        std::shared_lock lock(m_mutex);        
        return m_valid;
    }

    bool Expression::getRectMode() const {
        std::shared_lock lock(m_mutex);        
        return m_rectMode;
    }

    primitives::PrimitiveTypes Expression::getPrimitiveType() const {
        std::shared_lock lock(m_mutex);        
        return m_primitiveType;
    }

    void Expression::setPrimitiveType(math::primitives::PrimitiveTypes type) {
        std::unique_lock lock(m_mutex);
        m_primitiveType = type;
    }

    void Expression::setRectMode(bool rectMode) {
        std::unique_lock lock(m_mutex);
        m_rectMode = rectMode;
    }

    std::shared_ptr<const std::vector<std::vector<glm::dvec2>>> Expression::getComplexGrid() const {
        return m_complexGrid.front();
    }

    std::shared_ptr<const std::vector<glm::dvec2>> Expression::getPlotBuffer() const {
        return m_plotBuffer.front();
    }

    std::string Expression::getLastErrorMessage() const {
        std::shared_lock lock(m_mutex);        
        return m_lastErrorMessage;
    }
}