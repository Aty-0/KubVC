#pragma once
#include "singleton.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/implot.h"

#include "IconsFontAwesome6.h"

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
            inline ImFont* getIconFont() const { return m_iconFont; }

            // Default font but with math font size 
            inline ImFont* getDefaultFontMathSize() const { return m_defaultFontMathSize; }

        private:
            ImFont* m_mathFont;
            ImFont* m_iconFont;
            ImFont* m_defaultFont;
            ImFont* m_defaultFontMathSize;

            void applyColorTheme();
    };

    static inline auto toImVec4(glm::vec4 vec)
    {
        return ImVec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toImVec2(glm::vec2 vec)
    {
        return ImVec2(vec.x, vec.y);
    }

    static inline auto toGlmVec4(ImVec4 vec)
    {
        return glm::vec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toGlmVec2(ImVec2 vec)
    {
        return glm::vec2(vec.x, vec.y);
    }
}