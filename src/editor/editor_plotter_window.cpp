#include "editor_plotter_window.h"
#include "expression_controller.h"
#include "vec_convert.h"
#include "application_config.h"

namespace kubvc::editor {
    static const auto controller = math::ExpressionController::getInstance();

    EditorPlotterWindow::EditorPlotterWindow() {
        setName("Viewer##EditorPlotterWindow");
    }

    void EditorPlotterWindow::onRender([[maybe_unused]] kubvc::render::GUI& gui) {
        static constexpr auto vecStride = 2 * sizeof(double);
        static constexpr auto plotFlags = ImPlotFlags_::ImPlotFlags_NoTitle | ImPlotFlags_::ImPlotFlags_Crosshairs;
        const auto size = ImGui::GetContentRegionAvail();
        static bool saveLimitsFirstTime = false; 
        
        if (ImPlot::BeginPlot("##PlotViewer", size, plotFlags)) {

            // Draw axis notes 
            static const auto appConfig = application::ApplicationConfig::getInstance();
            
            static constexpr auto PLOT_AXIS_FLAGS = ImPlotAxisFlags_::ImPlotAxisFlags_Foreground;

            switch (appConfig->getMode()) {
                case application::MathMode::Real: {
                    ImPlot::SetupAxis(ImAxis_X1, "X-Axis", PLOT_AXIS_FLAGS);
                    ImPlot::SetupAxis(ImAxis_Y1, "Y-Axis", PLOT_AXIS_FLAGS);
                    break;
                }
                case application::MathMode::Complex: {
                    ImPlot::SetupAxis(ImAxis_X1, "Re", PLOT_AXIS_FLAGS);
                    // TODO: SetupAxisTicks
                    ImPlot::SetupAxis(ImAxis_Y1, "Im", PLOT_AXIS_FLAGS);
                    break;                    
                }
            }
            
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
            const auto& models = controller->getValidExpressions(); 
            for (std::size_t i = 0; i < models.size(); ++i) {
                const auto& model = models[i];
                if (!model) {
                    continue;
                }

                const auto& settings = model->getSettings(); 
                const auto& textBuffer = model->getTextBuffer(); 
                const auto& expression = model->getExpression();

                if (settings->getVisible() && expression->isValid()) { 
                    if (updateExpressions) {
                        if (appConfig->getMode() == application::MathMode::Real) {
                            math::GraphLimits::GlobalLimits = ImPlot::GetPlotLimits();   
                            controller->evalExpression(expression, math::GraphLimits::GlobalLimits);
                        }
                    }
                    
                    const auto isShaded = settings->getShaded() ? ImPlotLineFlags_::ImPlotLineFlags_Shaded : ImPlotLineFlags_::ImPlotLineFlags_None;
                    const auto flags = ImPlotLineFlags_::ImPlotLineFlags_NoClip | isShaded;
                    const auto& bufferPtr = expression->getPlotBuffer(); 

                    // Apply plot style from expression                                                   
                    ImPlotSpec specs;
                    specs.LineWeight = settings->getThickness();
                    specs.LineColor = kubvc::utility::toImVec4(settings->getColor());
                    specs.Flags = flags;
                    specs.Stride = vecStride;
                    
                    switch (appConfig->getMode()) {
                        case application::MathMode::Real: {
                            if (bufferPtr) {
                                const auto& buffer = *bufferPtr;
                                if (!buffer.empty()) {
                                    ImPlot::PlotLine(textBuffer->getBuffer().data(), &buffer[0].x, &buffer[0].y, 
                                        static_cast<std::int32_t>(buffer.size()), specs);      
                                }
                            }
                            break;
                        }
                        case application::MathMode::Complex: {
                            const auto isRectMode = expression->getRectMode();
                            if (!isRectMode) {
                                if (bufferPtr) {
                                    const auto& buffer = *bufferPtr;
                                    if (!buffer.empty()) {
                                        ImPlot::PlotLine(textBuffer->getBuffer().data(), &buffer[0].x, &buffer[0].y, 
                                            static_cast<std::int32_t>(buffer.size()), specs);      
                                    }
                                }
                            } else {
                                const auto& gridPtr = expression->getComplexGrid();
                                if (gridPtr) {
                                    const auto& grid = *gridPtr;                                
                                    for (const auto& lines : grid) {
                                        ImPlot::PlotLine(textBuffer->getBuffer().data(), &lines[0].x, &lines[0].y, 
                                            static_cast<std::int32_t>(lines.size()), specs);      
                                    }
                                }
                            }
                            break;                    
                        }
                    }
                    //ImPlot::PlotScatter(expr->getTextBuffer().data(), &buffer[0].x, &buffer[0].y, buffer.size(), flags, 0, vecStride);                                                      
                }                    
            }                
            
            updateExpressions = false;
            ImPlot::EndPlot();

        }
    }
}