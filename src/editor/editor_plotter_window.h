#pragma once
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorPlotterWindow : public EditorWindow {
        EditorPlotterWindow();    
        virtual void onRender(kubvc::render::GUI& gui) final;       
    };
}