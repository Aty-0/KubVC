#include "editor_keyboard_window.h"
#include <string>
#include <cstring>
#include "../expression.h"

namespace kubvc::editor {
    EditorKeyboardWindow::EditorKeyboardWindow() {
        setName("Keyboard");
        m_disableVisibleToggle = true;
        setVisible(false);
    }

    static auto drawPickElementButton(const std::string& text, const ImVec2& size) -> bool {
        auto cText = text.c_str();
    
        if (ImGui::Button(cText, size)) {
            auto selected = kubvc::math::ExpressionController::Selected;
            if (selected != nullptr) {
                const auto len = std::strlen(cText);
                auto end = cText + len;
                auto& buffer = selected->getTextBuffer();
                // Find last empty character in buffer 
                auto beg = buffer.begin() + selected->getCursor();
                
                buffer.insert(beg, cText, end);
                
                selected->setCursor(selected->getCursor() + len);        
                selected->parseThenEval(math::GraphLimits::Limits);
            }
    
            return true;
        }    
    
        return false;
    }

    auto EditorKeyboardWindow::onRender(kubvc::render::GUI* gui) -> void {
        // TODO: Operators, functions as childs
        if (ImGui::Button("Functions")) {
            ImGui::OpenPopup("FunctionsKeyboardPopup");
        }
        if (ImGui::BeginPopup("FunctionsKeyboardPopup")) { 
            drawFunctions();
            ImGui::EndPopup();
        }
        
        const auto childFlags = ImGuiChildFlags_::ImGuiChildFlags_Borders;
        const auto childWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar |  ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding;

        static const auto keyboardTableFlags = ImGuiTableFlags_::ImGuiTableFlags_Reorderable | ImGuiTableFlags_::ImGuiTableFlags_Hideable 
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
                drawKeys();
            }
            ImGui::EndChild();
        }
        ImGui::EndTable();        
    }    

    auto EditorKeyboardWindow::drawOperators() -> void {
        const auto opColumnsCount = 6;
        ImGui::TextDisabled("Operators");
        ImGui::Separator();

        if (ImGui::BeginTable("opTable", opColumnsCount)) {                        
            static const std::initializer_list<unsigned char> ops = { '+', '-', '*', '/', '^', '=' };
            const auto opButtonSize = ImVec2(35.0f, 35.0f);
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

    auto EditorKeyboardWindow::drawKeys() -> void {
        const auto opColumnsCount = 6;
        const auto opButtonSize = ImVec2(35.0f, 35.0f);        
        ImGui::TextDisabled("Keys");
        ImGui::Separator(); 
        static bool isUp = false;    
        if (ImGui::Button("Up", opButtonSize)) {
            isUp = !isUp;
        }

        ImGui::SameLine();
        if (ImGui::Button("<-", opButtonSize)) {
            auto selected = kubvc::math::ExpressionController::Selected;
            auto cursor = selected->getCursor();

            if (cursor > 0) {
                // Remove character by cursor from text buffer 
                auto& buffer = selected->getTextBuffer();
                auto begin = buffer.begin() + cursor;
                buffer.erase(begin - 1, begin);

                selected->setCursor(cursor - 1);

                // Update  
                selected->parseThenEval(math::GraphLimits::Limits);
            }
        }

        if (ImGui::BeginTable("keysTable", opColumnsCount)) {                        
            // Very, a very dumb way to implement qwerty keyboard
            // Maybe not, idk
            const std::int8_t QWERTY_KEYS_SIZE = 26; 
            const char* QWERTY_KEYS_UP = "QWERTYUIOPASDFGHJKLZXCVBNM";
            const char* QWERTY_KEYS_DOWN = "qwertyuiopasdfghjklzxcvbnm";

            for (std::uint8_t i = 0; i < QWERTY_KEYS_SIZE; i++) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(std::string(1, isUp ? QWERTY_KEYS_UP[i] : QWERTY_KEYS_DOWN[i]), opButtonSize);                
            }

            ImGui::EndTable();
        }
    }

    auto EditorKeyboardWindow::drawNumbers() -> void {
        const auto opColumnsCount = 6;
        ImGui::TextDisabled("Numbers");
        ImGui::Separator();

        if (ImGui::BeginTable("numTable", opColumnsCount)) {                        
            const auto opButtonSize = ImVec2(35.0f, 35.0f);
            for (char i = '0'; i <= '9'; i++) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(std::string(1, i), opButtonSize);                
            }

            ImGui::EndTable();
        }

        ImGui::TextDisabled("Constants");
        ImGui::Separator();

        if (ImGui::BeginTable("constTable", opColumnsCount)) {                        
            const auto opButtonSize = ImVec2(35.0f, 35.0f);
            for (auto i : kubvc::math::containers::Constants) {         
                ImGui::TableNextColumn();       
                drawPickElementButton(i.first, opButtonSize);                
            }

            ImGui::EndTable();
        }
    }

    auto EditorKeyboardWindow::drawFunctions() -> void {
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::TextDisabled("Functions");
        ImGui::Separator();

        const auto funcColumnsCount = 5;
        if (ImGui::BeginTable("funcTable", funcColumnsCount)) {     
            std::int32_t itemCount = 0;                   
            for (auto item : kubvc::math::containers::Functions) {         
                if (itemCount % funcColumnsCount == 0 && itemCount != 0) {
                    ImGui::TableNextRow();
                }

                ImGui::TableNextColumn();       
                if (drawPickElementButton(item.first, ImVec2(0.0f, 35.0f))) {
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
