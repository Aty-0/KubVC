#include "expression.h"
#include "logger.h"
#include "expression_controller.h"
#include "application_config.h"

namespace kubvc::math {
    Expression::Expression()  : 
        m_tree(), 
        m_plotBuffer(std::vector<glm::dvec2>(MAX_PLOT_BUFFER_SIZE)),
        m_valid(false),
        m_lastErrorMessage(),
        m_primitiveType(primitives::PrimitiveTypes::Circle),
        m_rectMode(false) {
            // TODO: Prepare only when we are switch (already) in complex mode
            prepareComplexGrid();
            // Set default primitive
            setNewPrimitive(std::move(primitives::makeNewPrimitive<primitives::CirclePrimitive>(MAX_PLOT_BUFFER_SIZE)));
    }

    Expression::~Expression() {    
        std::unique_lock lock(m_mutex); 
        m_valid = false;
        m_vdc.reset();
        m_tree.clear();
        
        m_plotBuffer.clear();
        //m_plotBuffer.shrink_to_fit();

        m_complexGrid.clear();
        //m_complexGrid.shrink_to_fit();
    }

    static constexpr std::uint8_t NEWTON_MAX_ITER = 16; 
    static constexpr auto EPS = 1e-10;

    inline static double solveNewton(std::function<double(double)> f, double min, double max) {        
        const auto diff = max - min;
        const auto eps_step = 1e-7 * diff;
        const auto eps_abs = 1e-10 * diff;
        auto x0 = (min + max) / 2.0;
        auto fx0 = std::numeric_limits<double>::max();
        auto dfx0 = std::numeric_limits<double>::max();        
        for (std::int32_t i = 0; i < NEWTON_MAX_ITER; ++i) {
            if (i > 0 || dfx0 == std::numeric_limits<double>::max()) {
                if (i > 0 || fx0 == std::numeric_limits<double>::max()) {
                    fx0 = f(x0);
                    if (glm::isnan(fx0)) {
                        return std::numeric_limits<double>::quiet_NaN();
                    }
                }

                dfx0 = (f(x0 + EPS) - f(x0 - EPS)) / (2.0 * EPS);    
                if (glm::isnan(dfx0) || dfx0 == 0.0) { // FIXME: dfx0 == 0.0
                    return std::numeric_limits<double>::quiet_NaN();
                }
            }
            double delta = fx0 / dfx0;
            x0 -= delta;
            if (glm::abs(delta) < eps_step && glm::abs(fx0) < eps_abs) {
                return x0;
            }
            
            if (x0 < min || x0 > max) {
                return std::numeric_limits<double>::quiet_NaN();
            }
        }
        return std::numeric_limits<double>::quiet_NaN();
    }

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
    
    void Expression::prepareComplexGrid() {
        m_complexGrid.resize(COMPLEX_GRID_SIZE, std::vector<glm::dvec2>(COMPLEX_GRID_LINES_COUNT));
    }

    void Expression::eval(const GraphLimits& limits, std::int32_t maxPointCount) {   
        if (!isValid())
            return; 
        static const auto appConfig = application::ApplicationConfig::getInstance();
        static const auto controller = ExpressionController::getInstance();        
        static auto& taskManager = controller->getTaskManager();
        
        std::shared_lock lock(m_mutex);        
        const auto root = m_tree.getRoot();
        KUB_ASSERT(root != nullptr, "Root is nullptr, wtf");            
        if (!root->child) {
            KUB_WARN("child in root is empty");
            return;
        }
        lock.unlock();

        switch (appConfig->getMode()) {
            case application::MathMode::Complex: {
                taskManager.add([this, root, limits, maxPointCount] {
                    if (!isValid())
                        return; 

                    if (m_rectMode) { 
                        for (std::size_t i = 0; i < COMPLEX_GRID_SIZE; ++i) {
                            const auto x = std::lerp(limits.xMin, limits.xMax, static_cast<double>(i) / (COMPLEX_GRID_SIZE - 1));
                            for (std::size_t j = 0; j < COMPLEX_GRID_LINES_COUNT; ++j) {
                                const auto y = std::lerp(limits.yMin, limits.yMax, static_cast<double>(j) / (COMPLEX_GRID_LINES_COUNT - 1));
                                const auto w = root->calculateComplex(x, y);
                                
                                std::unique_lock lock(m_mutex);
                                m_complexGrid[i][j] = { w.real(), w.imag() };
                            }
                        }
                    } else {                        
                        if (!m_primitive) {
                            return;
                        }
                        const auto points = m_primitive->getPoints();
                        for (std::size_t i = 0; i < points.size(); ++i) {                            
                            const auto point = points[i];
                            const auto w = root->calculateComplex(point.x, point.y);
                            
                            std::unique_lock lock(m_mutex);
                            m_plotBuffer[i] = { w.real(), w.imag() };
                        }                     
                    }                    
                });
                break;        
            }
            case application::MathMode::Real: {
                taskManager.add([this, root, limits, maxPointCount] { 
                    if (!isValid())
                        return; 

                    const auto left = m_vdc.getVariableAtSide(math::VDC::VariableSide::Left);
                    const bool isYPrefered = !left.has_value() || left.value().value == 'y';

                    for (std::int32_t i = 0; i < maxPointCount; ++i) {                              
                        if (isYPrefered) {                    
                            const auto x0 = std::lerp(limits.xMin, limits.xMax, static_cast<double>(i) / (maxPointCount - 1));
                            const auto f = [root, x0](const double y) {
                                double out = 0.0;
                                root->calculate(x0, y, out);
                                return out - y;
                            };  

                            auto y0 = solveNewton(f, limits.yMin, limits.yMax);
                            if (glm::isnan(y0)) {
                                y0 = solveBisection(f, limits.yMin, limits.yMax);
                            }

                            std::unique_lock lock(m_mutex);
                            m_plotBuffer[i] = { x0, y0 };
                        } else {
                            const auto y0 = std::lerp(limits.yMin, limits.yMax, static_cast<double>(i) / (maxPointCount - 1));
                            const auto f = [root, y0](const double x) {
                                double out = 0.0;
                                root->calculate(x, y0, out);
                                return out - x;
                            };  

                            auto x0 = solveNewton(f, limits.xMin, limits.xMax);
                            if (glm::isnan(x0)) {
                                x0 = solveBisection(f, limits.xMin, limits.xMax);
                            }
                            std::unique_lock lock(m_mutex);
                            m_plotBuffer[i] = { x0, y0 };
                        }
                    }
                });        
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
}