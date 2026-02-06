#include "editor_plotter_window.h"
#include "expression.h"
#include "vec_convert.h"

//#define SHOW_DEBUG_LIMITS 

namespace kubvc::editor {
    EditorPlotterWindow::EditorPlotterWindow() {
        setName("Viewer");
    }

    static void updateExpressionByPlotLimits(std::shared_ptr<kubvc::math::Expression> expr) {
        math::GraphLimits::Limits = ImPlot::GetPlotLimits();                 
        expr->eval(math::GraphLimits::Limits);
    }

    void EditorPlotterWindow::onRender(kubvc::render::GUI* gui) {
        static constexpr auto vecStride = 2 * sizeof(double);
        auto size = ImGui::GetContentRegionAvail();
        const auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;
        if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) {
            static bool savedFirstLimits = false; 

            // Draw axis notes 
            ImPlot::SetupAxis(ImAxis_X1, "X-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
            ImPlot::PushStyleColor(ImPlotCol_::ImPlotCol_AxisBgActive, ImVec4(255,0,0,255));
            ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
    
            static bool updateExpr = false;
            if (ImPlot::IsPlotHovered()) {
                static auto prevPos = ImPlotPoint(0, 0);
                auto pos = ImPlot::GetPlotLimits().Min(); 
    
                if (prevPos.x != pos.x || prevPos.y != pos.y) {
                    updateExpr = true;
                }
                
                prevPos = pos;
            }
            
            if (!savedFirstLimits) {
                math::GraphLimits::Limits = ImPlot::GetPlotLimits(); 
                savedFirstLimits = true;
            }	
#ifdef SHOW_DEBUG_LIMITS
            const auto limits = ImPlot::GetPlotLimits();    
            const glm::dvec2 limitsMin = { limits.X.Min, limits.Y.Min };
            const glm::dvec2 limitsMax = { limits.X.Max, limits.Y.Max };
            ImPlot::PlotText("debug_limits_min", limitsMin[0] / 2, limitsMin[1] / 2);
            ImPlot::PlotScatter("debug_limits_min", &limitsMin[0], &limitsMin[1], 2, 0, 0, vecStride);      
            ImPlot::PlotText("debug_limits_max", limitsMax[0] / 2, limitsMax[1] / 2);
            ImPlot::PlotScatter("debug_limits_max", &limitsMax[0], &limitsMax[1], 2, 0, 0, vecStride);      
#endif

            // Draw our functions 
            for (auto expr : kubvc::math::ExpressionController::Expressions) {                    
                if (expr != nullptr) {
                    if (expr->isVisible() && expr->isValid()) { 
                        if (updateExpr) {
                            updateExpressionByPlotLimits(expr);        
                        }
    
                        auto buffer = expr->getPlotBuffer(); 
                        if (buffer.size() > 0) {
                            // Apply plot style from expression                                                   
                            ImPlot::SetNextLineStyle(kubvc::utility::toImVec4(expr->Settings.color), expr->Settings.thickness);    

                            const auto shaded = expr->Settings.shaded ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                            const auto plotLineFlags = ImPlotLineFlags_::ImPlotLineFlags_NoClip | shaded;
    
                            ImPlot::PlotLine(expr->getTextBuffer().data(), &buffer[0].x, &buffer[0].y, buffer.size(), plotLineFlags, 0, vecStride);      
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