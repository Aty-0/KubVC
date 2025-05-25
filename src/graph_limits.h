#pragma once

struct ImPlotRect;

namespace kubvc::math
{
    struct GraphLimits
    {
        GraphLimits();
        GraphLimits(const ImPlotRect& rect);

        GraphLimits& operator= (const ImPlotRect& l);
        
        double xMin;
        double xMax;
        double yMin;
        double yMax;
        
        static struct GraphLimits Limits;
    };

}