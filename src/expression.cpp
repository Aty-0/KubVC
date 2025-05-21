#include "expression.h"
#include "logger.h"
#include <random>

namespace kubvc::math
{
    Expression::~Expression()
    {
        DEBUG("Destroy expression id %d ...", m_id);
        m_visible = false;
        m_valid = false;
        m_tree.clear();
        m_id = -1;
        m_textBuffer.clear();
        m_textBuffer.shrink_to_fit();
        m_plotBuffer.clear();
        m_plotBuffer.shrink_to_fit();
    }

    void Expression::eval(double xMax, double xMin, double yMax, double yMin, std::int32_t maxPointCount)
    {    
        auto root = m_tree.getRoot();
        if (root->child == nullptr)
            return;

        static const auto f = [](std::shared_ptr<kubvc::algorithm::RootNode> root, double x)
        {
            double out = 0.0;
            root->calculate(x, out);
            return out;
        };

        for (std::int32_t i = 0; i < maxPointCount; ++i)
        {
            auto x = std::lerp(xMin, xMax, static_cast<double>(i) / (maxPointCount - 1));
            double y = f(root, x);

            // Discard points which get away from max and min limit
            if (y > yMax || y < yMin) 
            {
                y = std::numeric_limits<double>::quiet_NaN();
            }

            if (x > xMax || x < xMin) 
            {
                x = std::numeric_limits<double>::quiet_NaN();
            }

            // Save our vector 
            m_plotBuffer[i] = { x, y };
        }


    
        if (!Settings.isRandomColorSetted)
        {
            std::uniform_real_distribution<float> unif(0, 1.0f);
            std::random_device rd;
            std::default_random_engine re(rd());

            Settings.color = { unif(re), unif(re), unif(re), 1.0f };
            Settings.isRandomColorSetted = true;
        }
    }
}