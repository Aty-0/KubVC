#pragma once
#include "singleton.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/implot.h"

#include "IconsFontAwesome6.h"

namespace kubvc::render {
    class GUI : public utility::Singleton<GUI> {
        public:
            void init();
            void destroy();

            void beginDockspace();
            void endDockspace();

            void begin();
            void end();

            inline ImFont* getMathFont() const { return m_mathFont; }
            inline ImFont* getDefaultFont() const { return m_defaultFont; }
            inline ImFont* getIconFont() const { return m_iconFont; }

            // Default font but with math font size 
            inline ImFont* getDefaultFontMathSize() const { return m_defaultFontMathSize; }

            void applyDefaultKubDarkTheme();
            void applyImGuiWhiteTheme();
            void applyImGuiClassicTheme();
            void applyImGuiDarkTheme();

        private:
            ImFont* m_mathFont;
            ImFont* m_iconFont;
            ImFont* m_defaultFont;
            ImFont* m_defaultFontMathSize;

    };
}