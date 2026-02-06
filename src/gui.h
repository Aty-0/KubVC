#pragma once
#include "singleton.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/implot.h"

#include "IconsFontAwesome6.h"

namespace kubvc::render {
    class GUI : public utility::Singleton<GUI> {
        public:
            auto init() -> void;
            auto destroy() -> void;

            auto beginDockspace() -> void;
            auto endDockspace() -> void;

            auto begin() -> void;
            auto end() -> void;

            inline auto getMathFont() -> ImFont* const { return m_mathFont; }
            inline auto getDefaultFont() -> ImFont* const { return m_defaultFont; }
            inline auto getIconFont() -> ImFont* const { return m_iconFont; }

            // Default font but with math font size 
            inline auto getDefaultFontMathSize() -> ImFont* const { return m_defaultFontMathSize; }

            auto applyDefaultKubDarkTheme() -> void;
            auto applyImGuiWhiteTheme() -> void;
            auto applyImGuiClassicTheme() -> void;
            auto applyImGuiDarkTheme() -> void;

        private:
            ImFont* m_mathFont;
            ImFont* m_iconFont;
            ImFont* m_defaultFont;
            ImFont* m_defaultFontMathSize;

    };
}