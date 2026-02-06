#pragma once 
#include "editor/editor.h"
#include "expression.h"

namespace kubvc::editor {
    struct EditorGraphListWindow : public EditorWindow {
        EditorGraphListWindow();

        virtual auto onRender(kubvc::render::GUI* gui) -> void final;       
        private:
            static constexpr auto INVALID_COLOR = ImVec4(0.64f, 0.16f, 0.16f, 1.0f);             
            static constexpr auto SELECTED_COLOR = ImVec4(0.16f, 0.64f, 0.16f, 1.0f);                   

            static auto handleExpressionCursorPosCallback(ImGuiInputTextCallbackData* data) -> std::int32_t;
            
            auto drawGraphPanel(kubvc::render::GUI* gui, std::shared_ptr<kubvc::math::Expression> expr, 
                const std::int32_t& id, const std::int32_t& index) -> void;
            auto drawGraphList(kubvc::render::GUI* gui)-> void;            
            auto drawGraphListHeader() -> void;            
    };
}