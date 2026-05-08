#pragma once 
#include <vector>
#include "logger.h"

namespace kubvc::math {
    class ExpressionTextBuffer {
        public:
            static constexpr std::size_t MAX_BUFFER_SIZE = 256;
            
            ExpressionTextBuffer();
            ~ExpressionTextBuffer();

            void setCursor(std::size_t cursorPos);

            [[nodiscard]] std::size_t getCursor()  const { return m_cursor; }
            [[nodiscard]] std::vector<char>& getBuffer() { return m_textBuffer; } 
        private:
            // Current cursor position in text box 
            std::size_t m_cursor;
            // Text buffer for text box   
            std::vector<char> m_textBuffer; 
    };
    
    inline ExpressionTextBuffer::ExpressionTextBuffer() : m_cursor(0), m_textBuffer(std::vector<char>(MAX_BUFFER_SIZE)) {

    }

    inline ExpressionTextBuffer::~ExpressionTextBuffer() {
        m_cursor = 0;
        m_textBuffer.clear();
        m_textBuffer.shrink_to_fit();
    }

    inline void ExpressionTextBuffer::setCursor(std::size_t cursorPos) {
        KUB_ASSERT(cursorPos < m_textBuffer.size(), "Cursor is bigger than text buffer size");
        m_cursor = cursorPos; 
    }   
}