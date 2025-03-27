#pragma once
#include "singleton.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/implot.h"

namespace kubvc::render
{
    class GUI : public utility::Singleton<GUI> 
    {
        public:
            void init();
            void destroy();

            void beginDockspace();
            void endDockspace();

            void begin();
            void end();

            inline ImFont* getMathFont() const { return m_mathFont; }
            inline ImFont* getDefaultFont() const { return m_defaultFont; }

            // Default font but with math font size 
            inline ImFont* getDefaultFontMathSize() const { return m_defaultFontMathSize; }

        private:
            ImFont* m_mathFont;
            ImFont* m_defaultFont;
            ImFont* m_defaultFontMathSize;

            void applyColorTheme();
    };
}