#pragma once 
#include "io.h"
#include "expression_controller.h"
#include "logger.h"
#include <format>
#include <ranges>

namespace kubvc::io {
    class ExpressionIO : public utility::Singleton<ExpressionIO> {
        public:
            // Save all points from plot buffer to file 
            void saveGraphPoints(std::string_view path, std::shared_ptr<math::ExpressionModel> expression);
            // Save all graphs to file 
            void saveGraphs(std::string_view path);
            // Load and evaluate graphs from file
            void loadGraphs(std::string_view path);
    };

    inline void ExpressionIO::saveGraphPoints(std::string_view path, std::shared_ptr<math::ExpressionModel> expression) {
        if (expression != nullptr) {
            io::FileSaver file;
            std::vector<char> buffer; 
            for (const auto& point : expression->getExpression().getPlotBuffer()) {
                const auto str = std::format("{}, {}\n", point.x, point.y);
                buffer.insert(buffer.begin(),  str.begin(), str.end());
            }
            KUB_ASSERT(file.save(path, buffer), "failed to save file");
        }
    }

    inline void ExpressionIO::saveGraphs(std::string_view path) {
        static const auto controller = math::ExpressionController::getInstance();

        const auto expressions = controller->getExpressions();
        std::vector<char> fileContentBuffer;
        for (auto expression : expressions) {
            if (expression != nullptr) {
                const auto exprBuffer = expression->getTextBuffer().getBuffer();
                // Find end, because we are have a fixed-size buffer    
                const auto actualEnd = std::find(exprBuffer.begin(), exprBuffer.end(), '\0');                                                 
                fileContentBuffer.insert(fileContentBuffer.end(), exprBuffer.begin(), actualEnd);
                fileContentBuffer.push_back('\n');
            }
        }
        
        io::FileSaver file;
        KUB_ASSERT(file.save(path, fileContentBuffer), "failed to save file");  
    }

    inline void ExpressionIO::loadGraphs(std::string_view path) {
        static const auto controller = math::ExpressionController::getInstance();
        io::FileLoader file;
        const auto result = file.load(path);
        if (result.has_value()) {
            const auto& value = result.value();
            for (const auto& str : value | std::views::split('\n') | std::views::filter([](const auto& str) { return !str.empty(); })) {
                const auto newExpression = controller->create();
                // Add expression to buffer
                auto& buffer = newExpression->getTextBuffer().getBuffer();
                buffer.insert(buffer.begin(), str.begin(), str.end());
                // Try to parse it and evaluate 
                controller->parseThenEvaluate(newExpression, math::GraphLimits::GlobalLimits);
            }
        } else {
            KUB_ERROR("Trying to open a invalid file");
        }
    }

}