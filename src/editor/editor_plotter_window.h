#pragma once
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorPlotterWindow : public EditorWindow {
        EditorPlotterWindow();    
        virtual auto onRender(kubvc::render::GUI* gui) -> void final;       
    };
}