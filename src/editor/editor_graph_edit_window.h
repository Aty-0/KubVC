#pragma once 
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorEditGraphWindow : public EditorWindow {
        EditorEditGraphWindow();
        virtual void onRender(kubvc::render::GUI* gui) final;       

        private:
            void drawLineColorPicker();

            static constexpr auto THICKNESS_MIN = 0.5f;            
            static constexpr auto THICKNESS_MAX = 10.0f;       
            static constexpr auto THICKNESS_SPEED = 0.1f; 
            

    };  
}