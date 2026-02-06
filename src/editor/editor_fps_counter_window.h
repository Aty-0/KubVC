#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorFpsCounterWindow : public EditorWindow {
        EditorFpsCounterWindow();
        virtual auto onRender(kubvc::render::GUI* gui) -> void final;       
    };
}