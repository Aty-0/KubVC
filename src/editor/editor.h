#pragma once 
#include "gui.h"
#include "singleton.h"
#include <set>

namespace kubvc::editor
{
    struct EditorDrawable
    {
        virtual void render(kubvc::render::GUI* gui) = 0;
    };

    struct EditorWindow : public EditorDrawable
    {
        EditorWindow() : m_visible(true), m_name("untitled_editor_window"), 
            m_disableVisibleToggle(true), m_flags(0) { }

        inline virtual void render(kubvc::render::GUI* gui) final
        {
            if (!(m_visible && m_disableVisibleToggle))
                return;                

            if (ImGui::Begin(m_name, 
                m_disableVisibleToggle ? nullptr : &m_visible, m_flags))
            {
                onRender(gui);
            }

            ImGui::End();
        }

        inline bool isVisible() const { return m_visible; }
        inline ImGuiWindowFlags getWindowFlags() const { return m_flags; }

        inline void setVisible(bool visible) { m_visible = visible; }
        inline void setWindowFlags(ImGuiWindowFlags flags) { m_flags = flags; }

        protected:
            virtual void onRender(kubvc::render::GUI* gui) { }
            inline void setName(const char* name) { m_name = name; }
            
            ImGuiWindowFlags m_flags;
            const char* m_name;
            bool m_disableVisibleToggle;
            bool m_visible;
    };


    class Editor : public kubvc::utility::Singleton<Editor>
    {
        public:
            Editor();
            ~Editor();

            void render(kubvc::render::GUI* gui);

        private:
            std::set<std::shared_ptr<EditorDrawable>> m_windows;
    };
}