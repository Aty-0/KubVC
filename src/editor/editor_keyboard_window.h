#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorKeyboardWindow : public EditorWindow {
        EditorKeyboardWindow();
        virtual auto onRender(kubvc::render::GUI* gui) -> void final;       

        private:
            auto drawOperators() -> void;
            auto drawKeys() -> void;
            auto drawNumbers() -> void;
            auto drawFunctions() -> void;
    };
}