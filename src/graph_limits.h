#pragma once
#include "Libs/imgui/implot.h"

namespace kubvc::math {
    struct GraphLimits {
        constexpr GraphLimits() = default;
        constexpr GraphLimits(double xmin, double xmax, double ymin, double ymax);
        constexpr GraphLimits(const ImPlotRect& rect);

        constexpr GraphLimits& operator= (const ImPlotRect& l);
        
        double xMin = 0.0;
        double xMax = 1.0;
        double yMin = 0.0;
        double yMax = 1.0;        
        
        // FIXME: Bad
        static GraphLimits GlobalLimits;
    };

    inline GraphLimits GraphLimits::GlobalLimits = { };
    
    inline constexpr GraphLimits::GraphLimits(double xmin, double xmax, double ymin, double ymax) : xMin(xmin), xMax(xmax), yMin(ymin), yMax(ymax) { }
    inline constexpr GraphLimits::GraphLimits(const ImPlotRect& rect) : xMin(rect.X.Min), 
            xMax(rect.X.Max), yMin(rect.Y.Min), yMax(rect.Y.Max) {}

    inline constexpr GraphLimits& GraphLimits::operator= (const ImPlotRect& l) {
        xMax = l.X.Max;  
        xMin = l.X.Min;  
        yMax = l.Y.Max; 
        yMin = l.Y.Min;  
        
        return *this;
    }

}