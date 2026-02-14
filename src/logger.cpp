#include "logger.h"
#include <stdexcept>

#define KUB_USE_ANSI_COLORS

namespace kubvc::utility {       
    static inline std::string_view levelToStr(Logger::LogLevel level) {
        switch (level) {
            case Logger::LogLevel::Debug:
                return "debug";
            case Logger::LogLevel::Warning:
                return "warning";
            case Logger::LogLevel::Error:
                return "error";
            case Logger::LogLevel::Fatal:
                return "fatal";                    
        }
        return "none";
    }

#ifdef KUB_USE_ANSI_COLORS
    static inline std::string_view levelToColorStr(Logger::LogLevel level) {
        switch (level) {
            case Logger::LogLevel::Debug:
                return "37";
            case Logger::LogLevel::Warning:
                return "33";
            case Logger::LogLevel::Error:
                return "31";
            case Logger::LogLevel::Fatal:
                return "41";                    
        }
        return "0";
    }
#endif

    Logger::~Logger() {
        m_buffer.str("");
        m_buffer.pubseekpos(0);
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    void Logger::printImpl(Logger::LogLevel level, const std::source_location source, std::string_view text) {
        const auto line = std::format("[{}] [{} line: {}]: {}\n", levelToStr(level), source.function_name(), source.line(), text.data());
        m_buffer.sputn(line.data(), line.size());

#ifdef KUB_USE_ANSI_COLORS
        std::printf("\033[%sm%s\033[0m", levelToColorStr(level), line.data());
#else
        std::printf(line.data());
#endif

        // Throw error if we are get a fatal log level 
        if (level == Logger::LogLevel::Fatal) {
            throw std::runtime_error("Fatal error");
        }
    }

    void Logger::save(std::string_view path) {
        m_file.open(path.data(), std::ios_base::out);
        if (!m_file.is_open()) {
            KUB_ERROR("Failed to save file.");
            return;
        }
        m_file << m_buffer.str();
        m_file.close();
    }

}