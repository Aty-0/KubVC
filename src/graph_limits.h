#pragma once

struct ImPlotRect;

namespace kubvc::math
{
    struct GraphLimits {
        GraphLimits();
        GraphLimits(const ImPlotRect& rect);

        auto operator= (const ImPlotRect& l) -> GraphLimits&;
        
        double xMin;
        double xMax;
        double yMin;
        double yMax;
        
        static struct GraphLimits Limits;
    };

}