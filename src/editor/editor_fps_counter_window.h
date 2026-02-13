#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorFpsCounterWindow : public EditorWindow {
        EditorFpsCounterWindow();
        virtual void onRender(kubvc::render::GUI* gui);       
    };
}