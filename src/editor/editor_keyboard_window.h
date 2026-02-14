#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorKeyboardWindow : public EditorWindow {
        EditorKeyboardWindow();
        virtual void onRender(kubvc::render::GUI& gui) final;       

        private:
            void drawOperators();
            void drawKeys();
            void drawNumbers();
            void drawFunctions();
    };
}