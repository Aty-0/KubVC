#pragma once
#include "editor.h"

namespace kubvc::editor {
    struct EditorMathElementListWindow : public EditorWindow {
        EditorMathElementListWindow();
        virtual void onRender(kubvc::render::GUI& gui);

        private:
            void addItemToExpression(std::string_view text);       
    };
}