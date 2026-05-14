#include "editor_plotter_window.h"
#include "expression_controller.h"
#include "vec_convert.h"


namespace kubvc::editor {
    static const auto controller = math::ExpressionController::getInstance();

    EditorPlotterWindow::EditorPlotterWindow() {
        setName("Viewer##EditorPlotterWindow");
    }

    void EditorPlotterWindow::onRender([[maybe_unused]] kubvc::render::GUI& gui) {
        static constexpr auto vecStride = 2 * sizeof(double);
        static constexpr auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;
        const auto size = ImGui::GetContentRegionAvail();
        if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) {
            static bool saveLimitsFirstTime = false; 

            // Draw axis notes 
            ImPlot::SetupAxis(ImAxis_X1, "X-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
            ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", ImPlotAxisFlags_::ImPlotAxisFlags_Foreground);
            
            static bool updateExpressions = false;
            if (ImPlot::IsPlotHovered()) {
                static auto prevPos = ImPlotPoint(0, 0);

                const auto pos = ImPlot::GetPlotLimits().Min(); 
                if (prevPos.x != pos.x || prevPos.y != pos.y) {
                    updateExpressions = true;
                }
                
                prevPos = pos;
            }
            
            if (!saveLimitsFirstTime) {
                math::GraphLimits::GlobalLimits = ImPlot::GetPlotLimits(); 
                saveLimitsFirstTime = true;
            }	

            // Draw our functions 
            for (auto model : controller->getValidExpressions()) {
                KUB_ASSERT(model != nullptr, "Some model in expression list are nullptr");          

                auto& settings = model->getSettings(); 
                auto& textBuffer = model->getTextBuffer(); 
                auto& expression = model->getExpression();
                if (settings.getVisible() && expression.isValid()) { 
                    if (updateExpressions) {
                        math::GraphLimits::GlobalLimits = ImPlot::GetPlotLimits();                 
                        expression.eval(math::GraphLimits::GlobalLimits);     
                    }

                    const auto buffer = expression.getPlotBuffer(); 
                    if (!buffer.empty()) {
                        // Apply plot style from expression                                                   
                        const auto isShaded = settings.getShaded() ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                        const auto flags = ImPlotLineFlags_::ImPlotLineFlags_NoClip | isShaded;

                        ImPlotSpec specs;
                        specs.LineWeight = settings.getThickness();
                        specs.LineColor = kubvc::utility::toImVec4(settings.getColor());
                        specs.Flags = flags;
                        specs.Stride = vecStride;

                        ImPlot::PlotLine(textBuffer.getBuffer().data(), &buffer[0].x, &buffer[0].y, 
                            static_cast<std::int32_t>(buffer.size()), specs);      

                        //ImPlot::PlotScatter(expr->getTextBuffer().data(), &buffer[0].x, &buffer[0].y, buffer.size(), flags, 0, vecStride);                                  
                    }
                }                    
            }                

            updateExpressions = false;
            ImPlot::EndPlot();

        }
    }
}