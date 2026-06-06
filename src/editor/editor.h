#pragma once 
#include "gui.h"
#include "singleton.h"
#include <set>
#include <algorithm>
#include <string>

namespace kubvc::editor {
    struct EditorDrawable {
        virtual void render(kubvc::render::GUI& gui) = 0;
        virtual void renderMenuBarButton(kubvc::render::GUI& gui) = 0;
    };

    struct EditorWindow : public EditorDrawable {
        EditorWindow() : m_flags(0),
            m_name("untitled_editor_window"), 
            m_disableVisibleToggle(true),
            m_visible(true),
            m_iconName(ICON_FA_WAVE_SQUARE),
            m_iconDesc() { }

        virtual void render(kubvc::render::GUI& gui) final;
        virtual void renderMenuBarButton(kubvc::render::GUI& gui) final;

        [[nodiscard]] bool isVisible() const { return m_visible; }
        [[nodiscard]] ImGuiWindowFlags getWindowFlags() const { return m_flags; }

        void setVisible(bool visible) { m_visible = visible; }
        void setWindowFlags(ImGuiWindowFlags flags) { m_flags = flags; }
        void setName(std::string_view name) { m_name = name; }
        void setIconName(std::string_view name) { m_iconName = name; }
        void setIconDesc(std::string_view desc) { m_iconDesc = desc; }

        protected:
            ImGuiWindowFlags m_flags;
            std::string m_name;
            bool m_disableVisibleToggle;
            bool m_visible;

            std::string m_iconName;
            std::string m_iconDesc;

            virtual void onRender([[maybe_unused]] kubvc::render::GUI& gui) { }
    };

    inline void EditorWindow::renderMenuBarButton(kubvc::render::GUI& gui) {
        if (!m_disableVisibleToggle) {
            return;
        }
        auto active = m_visible;
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        }

        ImGui::PushFont(&gui.getIconFont());
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::Button((m_iconName + "##" + m_name).c_str())) {
            setVisible(!m_visible);
        }
        ImGui::PopStyleVar();
        ImGui::PopFont();
        
        if (active) {
            ImGui::PopStyleColor(2);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(m_iconDesc.empty() ? (m_name + " TODO: Make Desc").c_str() : m_iconDesc.c_str());
        }
    }

    inline void EditorWindow::render(kubvc::render::GUI& gui) {
        if (!(m_visible && m_disableVisibleToggle))
            return;      
        const auto name = m_name.c_str();
        if (ImGui::Begin(name, m_disableVisibleToggle ? nullptr : &m_visible, m_flags)) {
            onRender(gui);
        }
        ImGui::End();
    }



    class Editor : public kubvc::utility::Singleton<Editor> {
        public:
            Editor();
            ~Editor();

            void render(kubvc::render::GUI& gui);
            void renderMenuBarButtons(kubvc::render::GUI& gui);

            template<typename T>
            [[nodiscard]] std::shared_ptr<T> get() const;
        private:
            std::set<std::shared_ptr<EditorDrawable>> m_windows;
    };

    template<typename T>
    inline std::shared_ptr<T> Editor::get() const {
        const auto it = std::find_if(m_windows.begin(), m_windows.end(), [](const std::shared_ptr<EditorDrawable>& draw) {
            return typeid(*draw) == typeid(T);
        });                
        return it != m_windows.end() ? std::static_pointer_cast<T>(*it) : nullptr;
    }  
}