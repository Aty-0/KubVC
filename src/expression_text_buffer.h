#pragma once 
#include <vector>
#include <string>
#include "logger.h"

namespace kubvc::math {
    class ExpressionTextBuffer {
        public:
            static constexpr std::size_t MAX_BUFFER_SIZE = 256;
            
            ExpressionTextBuffer();
            ~ExpressionTextBuffer();
            
            void insertAtCursor(std::string_view text);
            void setCursor(std::size_t cursorPos);

            [[nodiscard]] std::size_t getCursor()  const { return m_cursor; }
            [[nodiscard]] std::vector<char>& getBuffer() { return m_buffer; } 
        private:
            // Current cursor position in text box 
            std::size_t m_cursor;
            // Text buffer for text box   
            std::vector<char> m_buffer; 
    };
    
    inline ExpressionTextBuffer::ExpressionTextBuffer() : m_cursor(0), m_buffer(std::vector<char>(MAX_BUFFER_SIZE)) {

    }

    inline void ExpressionTextBuffer::insertAtCursor(std::string_view text) {
        const auto& beg = m_buffer.begin() + m_cursor;            
        m_buffer.insert(beg, text.begin(), text.end());   
        setCursor(m_cursor + text.size());  
    }

    inline ExpressionTextBuffer::~ExpressionTextBuffer() {
        m_cursor = 0;
        m_buffer.clear();
        m_buffer.shrink_to_fit();
    }

    inline void ExpressionTextBuffer::setCursor(std::size_t cursorPos) {
        KUB_ASSERT(cursorPos < m_buffer.size(), "Cursor is bigger than text buffer size");
        m_cursor = cursorPos; 
    }   
}