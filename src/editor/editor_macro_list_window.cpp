#include "editor_macro_list_window.h"
#include "../macro_controller.h"

namespace kubvc::editor {
    static constexpr std::size_t TEXT_BUFFER_SIZE = 128;
    static const auto macroController = algorithm::MacroController::getInstance();

    EditorMacroListWindow::EditorMacroListWindow() : 
        m_nameTextBuffer(TEXT_BUFFER_SIZE),
        m_valueTextBuffer(TEXT_BUFFER_SIZE),
        m_selectedId(std::nullopt) {
        setName("Macros##EditorMacroListWindow");
        setIconName(ICON_FA_CODE);
    }

    void EditorMacroListWindow::drawMacroButton(kubvc::render::GUI& gui, const algorithm::Macro& macro) {
        if (macro.name.empty()) {
            return;
        }
        
        ImGui::PushFont(&gui.getIconFont());
        ImGui::Text(ICON_FA_WAVE_SQUARE);
        ImGui::SameLine();
        ImGui::PopFont();
        const auto name = macro.name + "##EditorMacroListWindowSelectable";
        if (ImGui::Selectable(name.c_str())) {
            m_selectedId = macro.getId();
            const auto& result = macroController->getMacro(m_selectedId.value());
            KUB_ASSERT(result.has_value(), "failed to get macro, kekw");
            const auto& value = result.value().get();

            m_nameTextBuffer.assign(value.name.begin(), value.name.end());
            m_nameTextBuffer.resize(TEXT_BUFFER_SIZE);

            m_valueTextBuffer.assign(value.value.begin(), value.value.end());
            m_valueTextBuffer.resize(TEXT_BUFFER_SIZE);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Click to this item to edit it.");
        }
    }

    void EditorMacroListWindow::onRender(kubvc::render::GUI& gui) {
        static bool addMacroFailed = false;

        ImGui::Text("Name:");
        ImGui::InputText("##EditorMacroListWindowInputName", m_nameTextBuffer.data(), m_nameTextBuffer.size());

        if (addMacroFailed) {
            ImGui::SameLine();
            ImGui::PushFont(&gui.getIconFont());
            ImGui::Text(ICON_FA_BOMB); // TODO: Proper icon
            ImGui::PopFont();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Failed to add macro because macro with that name is exist in list!");
            }
        }

        ImGui::Text("Value:");
        ImGui::InputText("##EditorMacroListWindowInputValue", m_valueTextBuffer.data(), m_valueTextBuffer.size());

        ImGui::SeparatorText("Edit:");

        if (ImGui::Button("Add##EditorMacroListWindowAddButton")) {
            auto macro = algorithm::Macro { };
            macro.name = std::string(m_nameTextBuffer.data());
            macro.value = std::string(m_valueTextBuffer.data());
            addMacroFailed = !macroController->add(std::move(macro));
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete##EditorMacroListWindowDeleteButton")) {
            if (m_selectedId.has_value()) {
                macroController->remove(m_selectedId.value());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Save##EditorMacroListWindowSaveButton")) {
            if (m_selectedId.has_value()) {
                const auto& result = macroController->getMacro(m_selectedId.value());
                if (result.has_value()) {
                    auto& macro = result.value().get();
                    macro.name = std::string(m_nameTextBuffer.data());
                    macro.value = std::string(m_valueTextBuffer.data());
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear##EditorMacroListWindowClearButton")) {
            m_nameTextBuffer[0] = '\0';
            m_valueTextBuffer[0] = '\0';

            m_selectedId = std::nullopt;
        }

        const auto availRegion = ImGui::GetContentRegionAvail();
        const auto region = ImVec2 { availRegion.x, availRegion.y / 2 };
        if (ImGui::BeginListBox("##EditorMacroListWindowList", region)) {
            for (const auto& macro : macroController->getMacros()) {
                drawMacroButton(gui, macro);
            }
            ImGui::EndListBox();
        }
    }       
}