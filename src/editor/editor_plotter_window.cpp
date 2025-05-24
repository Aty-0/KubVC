#include "editor_plotter_window.h"
#include "expression.h"
#include "vec_convert.h"

namespace kubvc::editor
{
    EditorPlotterWindow::EditorPlotterWindow()
    {
        setName("Viewer");
    }

    static void updateExpressionByPlotLimits(std::shared_ptr<kubvc::math::Expression> expr)
    {
        auto limits = ImPlot::GetPlotLimits();                 
        expr->eval(limits.X.Max, limits.X.Min, limits.Y.Max, limits.Y.Min);
    }

    void EditorPlotterWindow::onRender(kubvc::render::GUI* gui)  
    {
        auto size = ImGui::GetContentRegionAvail();
        const auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;
        if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) 
        {	
            // Draw axis notes 
            ImPlot::SetupAxis(ImAxis_X1, "X-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
            ImPlot::PushStyleColor(ImPlotCol_::ImPlotCol_AxisBgActive, ImVec4(255,0,0,255));
            ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
    
            static bool updateExpr = false;
            if (ImPlot::IsPlotHovered())
            {
                static auto prevPos = ImPlotPoint(0, 0);
                auto pos = ImPlot::GetPlotLimits().Min(); 
    
                if (prevPos.x != pos.x || prevPos.y != pos.y)
                {
                    updateExpr = true;
                }
                
                prevPos = pos;
            }
            
            // Draw our functions 
            for (auto expr : kubvc::math::ExpressionController::Expressions)
            {                    
                if (expr != nullptr)
                {
                    if (expr->isVisible() && expr->isValid())
                    { 
                        if (updateExpr)
                        {
                            updateExpressionByPlotLimits(expr);        
                        }
    
                        auto buffer = expr->getPlotBuffer(); 
                        if (buffer.size() > 0)
                        {
                            // Apply plot style from expression                                                   
                            ImPlot::SetNextLineStyle(kubvc::utility::toImVec4(expr->Settings.color), expr->Settings.thickness);
    
                            static constexpr auto stride = 2 * sizeof(double);
    
                            const auto shaded = expr->Settings.shaded ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                            const auto plotLineFlags = ImPlotLineFlags_::ImPlotLineFlags_NoClip | shaded;
    
                            ImPlot::PlotLine(expr->getTextBuffer().data(), &buffer[0].x, &buffer[0].y, buffer.size(), plotLineFlags, 0, stride);      
                            //ImPlot::PlotScatter(expr->textBuffer.data(), &expr->plotBuffer[0].x, &expr->plotBuffer[0].y, expr->plotBuffer.size(), plotLineFlags, 0, stride);      
                        }
                    }
                }    
            }                        
            updateExpr = false;
            ImPlot::EndPlot();
        }
    }
}