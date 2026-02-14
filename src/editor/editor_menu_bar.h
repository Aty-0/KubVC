#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorMenuBar : public EditorDrawable {
        virtual void render(kubvc::render::GUI& gui) final;        
    };
}