#include "graph_limits.h"
#include "Libs/imgui/implot.h"

namespace kubvc::math
{
    GraphLimits GraphLimits::Limits;
    
    GraphLimits::GraphLimits() : xMin(0), xMax(0), yMin(0), yMax(0) {}
    GraphLimits::GraphLimits(const ImPlotRect& rect) : xMin(rect.X.Min), 
            xMax(rect.X.Max), yMin(rect.Y.Min), yMax(rect.Y.Max) {}
    
    
    auto GraphLimits::operator= (const ImPlotRect& l) -> GraphLimits&  {
        xMin = l.X.Min;  
        xMax = l.X.Max;  
        yMin = l.Y.Min;  
        yMax = l.Y.Max; 
        
        return *this;
    }
}