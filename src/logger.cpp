#include "logger.h"
#include <stdio.h>
#include <cstdarg>
#include <vector>
#include <string>
#include <stdexcept>


namespace kubvc::utility
{
    static const std::vector<const char*> LogLevelInStr = 
    {
            "debug",
            "warning",
            "error", 
            "fatal"
    };
    
    void Logger::print(const Logger::LogLevel& level, const std::source_location source, const char* fmt, ...)
    {
        if (Logger::LogLevel::LOG_LEVEL_SIZE != LogLevelInStr.size())
        {
            throw std::runtime_error("Enumerator and string list size is different");
        } 
        else if (level == Logger::LogLevel::LOG_LEVEL_SIZE)
        {
            return;
        }
        
        std::va_list args = { };
        va_start(args, fmt);
        const auto buffer_size = 1 + std::vsnprintf(nullptr, 0, fmt, args);        
        std::vector<char> buffer(buffer_size);
        std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
        va_end(args);

        // Print buffer         
        std::printf("[%s] [%s line: %d]: %s\n", LogLevelInStr[static_cast<int>(level)], source.function_name(), source.line(), buffer.data());

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