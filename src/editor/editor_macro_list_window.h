#pragma once 
#include "editor/editor.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace kubvc::algorithm {
    struct Macro;
}

namespace kubvc::editor {
    struct EditorMacroListWindow : public EditorWindow {
        EditorMacroListWindow();
        ~EditorMacroListWindow() = default;

        virtual void onRender(kubvc::render::GUI& gui) final;   
        private: 
            void drawMacroButton(kubvc::render::GUI& gui, const algorithm::Macro& macro);
            
            std::vector<char> m_nameTextBuffer;
            std::vector<char> m_valueTextBuffer;
            std::optional<std::int32_t> m_selectedId;
    };
}