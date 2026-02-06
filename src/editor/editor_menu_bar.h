#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorMenuBar : public EditorDrawable {
        virtual auto render(kubvc::render::GUI* gui) -> void final;        
    };
}