#pragma once
#include "editor/editor.h"

namespace kubvc::editor {
    struct EditorPlotterWindow : public EditorWindow {
        EditorPlotterWindow();    
        ~EditorPlotterWindow() = default;

        virtual void onRender(kubvc::render::GUI& gui) final;    

        void redrawGraphs();

        private:
            bool m_redrawGraphs;

    };
}