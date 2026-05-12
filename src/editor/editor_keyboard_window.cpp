#include "editor_keyboard_window.h"
#include "expression_controller.h"
#include "alg_helpers.h"

#include <string>

namespace kubvc::editor {
    static const auto controller = math::ExpressionController::getInstance();

    EditorKeyboardWindow::EditorKeyboardWindow() {
        setName("Keyboard##EditorKeyboardWindow");
        m_disableVisibleToggle = true;
        setVisible(false);
    }

    static bool drawPickElementButton(std::string_view text, const ImVec2& size) {
        const auto cText = text.data();
        if (ImGui::Button(cText, size)) {
            const auto selected = controller->getSelected();
            if (selected != nullptr) {
                auto& textBuffer = selected->getTextBuffer();
                textBuffer.insertAtCursor(text);
                controller->parseThenEvaluate(selected, math::GraphLimits::GlobalLimits);
            }
    
            return true;
        }    
    
        return false;
    }

    void EditorKeyboardWindow::onRender(kubvc::render::GUI& gui) {
        // TODO: Operators, functions as childs
        if (ImGui::Button("Functions")) {
            ImGui::OpenPopup("FunctionsKeyboardPopup");
        }
        if (ImGui::BeginPopup("FunctionsKeyboardPopup")) { 
            drawFunctions();
            ImGui::EndPopup();
        }
        
        constexpr auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        constexpr auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar /* |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding */;

        static constexpr auto keyboardTableFlags = ImGuiTableFlags_::ImGuiTableFlags_Reorderable | ImGuiTableFlags_::ImGuiTableFlags_Hideable 
            | ImGuiTableFlags_::ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_::ImGuiTableFlags_RowBg |  ImGuiTableFlags_::ImGuiTableFlags_Resizable
            | ImGuiTableFlags_::ImGuiTableFlags_PadOuterX;
            
        if (ImGui::BeginTable("##KeyboardTable", 4, keyboardTableFlags)) {                    
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("NumbersKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags)) { 
                drawNumbers();
            }
            ImGui::EndChild();

            ImGui::TableNextColumn();
            if (ImGui::BeginChild("OperatorsKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags)) { 
                drawOperators();
            }
            ImGui::EndChild();

            ImGui::TableNextColumn();
            if (ImGui::BeginChild("keysKeyboardChild", ImVec2(0, 0), childFlags, childWindowFlags)) { 
                drawKeys(gui);
            }
            ImGui::EndChild();
        }
        ImGui::EndTable();        
    }    

    void EditorKeyboardWindow::drawOperators() {
        constexpr auto opColumnsCount = 6;
        constexpr auto opButtonSize = ImVec2(35.0f, 35.0f);
        ImGui::TextDisabled("Operators");
        ImGui::Separator();

        if (ImGui::BeginTable("opTable", opColumnsCount)) {                        
            static constexpr std::initializer_list<algorithm::Helpers::uchar> ops = { '+', '-', '*', '/', '^', '=' };
            for (auto item : ops) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(std::string(1, item), opButtonSize);                
            }

            ImGui::TableNextColumn();       
            drawPickElementButton(std::string(1, '('), opButtonSize);                

            ImGui::TableNextColumn();       
            drawPickElementButton(std::string(1, ')'), opButtonSize);                

            ImGui::EndTable();
        }
    }

    void EditorKeyboardWindow::drawKeys(kubvc::render::GUI& gui) {
        constexpr auto opColumnsCount = 6;
        constexpr auto opButtonSize = ImVec2(35.0f, 35.0f);        
        static bool isUp = false;    
        
        ImGui::TextDisabled("Keys");
        ImGui::Separator(); 
        if (ImGui::Button("Up", opButtonSize)) {
            isUp = !isUp;
        }

        ImGui::SameLine();

        ImGui::PushFont(&gui.getIconFont());
        if (ImGui::Button(ICON_FA_DELETE_LEFT, opButtonSize)) {
            const auto selected = controller->getSelected();
            if (selected != nullptr) {
                auto& textBuffer = selected->getTextBuffer();
                const auto cursor = textBuffer.getCursor();

                if (cursor > 0) {
                    // Remove character by cursor from text buffer 
                    auto& buffer = textBuffer.getBuffer();
                    const auto& begin = buffer.begin() + cursor;
                    buffer.erase(begin - 1, begin);
                    textBuffer.setCursor(cursor - 1);
                    controller->parseThenEvaluate(selected, math::GraphLimits::GlobalLimits);
                }                
            }
        }
        ImGui::PopFont();

        if (ImGui::BeginTable("keysTable", opColumnsCount)) {                        
            static constexpr std::string_view QWERTY_KEYS_DOWN = "qwertyuiopasdfghjklzxcvbnm";
            static constexpr std::int8_t QWERTY_KEYS_SIZE = static_cast<std::int8_t>(QWERTY_KEYS_DOWN.size()); 
            static const std::string QWERTY_KEYS_UP = algorithm::Helpers::toUpperCase(QWERTY_KEYS_DOWN);

            for (std::uint8_t i = 0; i < QWERTY_KEYS_SIZE; i++) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(std::string(1, isUp ? QWERTY_KEYS_UP[i] : QWERTY_KEYS_DOWN[i]), opButtonSize);                
            }

            ImGui::EndTable();
        }
    }

    void EditorKeyboardWindow::drawNumbers() {
        constexpr auto opColumnsCount = 6;
        constexpr auto opButtonSize = ImVec2(35.0f, 35.0f);
        ImGui::TextDisabled("Numbers");
        ImGui::Separator();

        if (ImGui::BeginTable("numTable", opColumnsCount)) {                        
            for (char i = '0'; i <= '9'; i++) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(std::string(1, i), opButtonSize);                
            }

            ImGui::EndTable();
        }

        ImGui::TextDisabled("Constants");
        ImGui::Separator();

        if (ImGui::BeginTable("constTable", opColumnsCount)) {                        
            for (const auto& [name, _] : kubvc::math::containers::Constants) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(name, opButtonSize);                
            }

            ImGui::EndTable();
        }
    }

    void EditorKeyboardWindow::drawFunctions() {
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::TextDisabled("Functions");
        ImGui::Separator();

        constexpr auto funcColumnsCount = 5;
        if (ImGui::BeginTable("funcTable", funcColumnsCount)) {     
            std::int32_t itemCount = 0;                   
            for (const auto& [name, _] : kubvc::math::containers::Functions) {         
                if (itemCount % funcColumnsCount == 0 && itemCount != 0) {
                    ImGui::TableNextRow();
                }

                ImGui::TableNextColumn();       
                if (drawPickElementButton(name, ImVec2(0.0f, 35.0f))) {
                    ImGui::CloseCurrentPopup();
                }

                ++itemCount;
            }
            ImGui::EndTable();
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
    }
} 
