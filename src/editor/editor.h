#pragma once 
#include "gui.h"
#include "singleton.h"
#include <set>
#include <algorithm>

namespace kubvc::editor {
    struct EditorDrawable {
        virtual auto render(kubvc::render::GUI* gui) -> void = 0;
    };

    struct EditorWindow : public EditorDrawable {
        EditorWindow() : m_visible(true), m_name("untitled_editor_window"), 
            m_disableVisibleToggle(true), m_flags(0) { }

        inline virtual auto render(kubvc::render::GUI* gui) -> void final {
            if (!(m_visible && m_disableVisibleToggle))
                return;                

            if (ImGui::Begin(m_name, 
                m_disableVisibleToggle ? nullptr : &m_visible, m_flags))
            {
                onRender(gui);
            }

            ImGui::End();
        }

        inline auto isVisible() -> bool const { return m_visible; }
        inline auto getWindowFlags() -> ImGuiWindowFlags const { return m_flags; }

        inline auto setVisible(bool visible) -> void { m_visible = visible; }
        inline auto setWindowFlags(ImGuiWindowFlags flags) -> void { m_flags = flags; }

        protected:
            virtual auto onRender(kubvc::render::GUI* gui) -> void { }
            inline auto setName(const char* name) -> void { m_name = name; }
            
            ImGuiWindowFlags m_flags;
            const char* m_name;
            bool m_disableVisibleToggle;
            bool m_visible;
    };


    class Editor : public kubvc::utility::Singleton<Editor> {
        public:
            Editor();
            ~Editor();

            auto render(kubvc::render::GUI* gui) -> void;

            template<typename T>
            inline auto get() -> std::shared_ptr<T> const {
                auto it = std::find_if(m_windows.begin(), m_windows.end(), [](const std::shared_ptr<EditorDrawable>& draw) {
                    return typeid(*draw) == typeid(T);
                });
                
                if (it != m_windows.end()) {
                    return std::static_pointer_cast<T>(*it);
                }

                return nullptr;
            }  
        private:
            std::set<std::shared_ptr<EditorDrawable>> m_windows;
    };
}