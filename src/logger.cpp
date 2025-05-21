#include "logger.h"
#include <stdio.h>
#include <cstdarg>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>


namespace kubvc::utility
{       
    static inline auto levelToStr(const Logger::LogLevel& level)
    {
        switch (level)
        {
            case Logger::LogLevel::Debug:
                return "debug";
            case Logger::LogLevel::Warning:
                return "warning";
            case Logger::LogLevel::Error:
                return "error";
            case Logger::LogLevel::Fatal:
                return "fatal";                    
        }
        return "wtf";
    }

    void Logger::print(const Logger::LogLevel& level, const std::source_location source, const char* fmt, ...)
    {
        if (level >= Logger::LogLevel::LOG_LEVEL_SIZE)
        {
            throw std::runtime_error("Invalid log level");
        }    

        std::va_list args = { };
        va_start(args, fmt);
        const auto buffer_size = 1 + std::vsnprintf(nullptr, 0, fmt, args);        
        std::vector<char> buffer(buffer_size);
        std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
        va_end(args);

        // Print buffer         
        std::printf("[%s] [%s line: %d]: %s\n", levelToStr(level), source.function_name(), source.line(), buffer.data());

//#endif
        // Clear buffer
        buffer.clear();
        buffer.shrink_to_fit();
        
        // Throw error if we are get a fatal log level 
        if (level == Logger::LogLevel::Fatal)
        {
            throw std::runtime_error("Fatal error");
        }
    }
}