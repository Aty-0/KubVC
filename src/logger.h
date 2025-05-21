#pragma once
#include "singleton.h"
#include <source_location>

namespace kubvc::utility
{
    class Logger : public Singleton<Logger>
    {
        public:
            enum LogLevel
            {
                Debug = 0,
                Warning,
                Error, 
                Fatal,

                LOG_LEVEL_SIZE 
            };

            void print(const Logger::LogLevel& level, const std::source_location source, const char* fmt, ...);
                        
        private:
    };

    static inline auto log = kubvc::utility::Logger::getInstance();
}

#define ASSERT(cond, fmt, ...) do {                                                                                                   \
        if (!cond)                                                                                                                    \
        {                                                                                                                             \
            ERROR("Assertion failed: ");                                                                                              \
            kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__); \
        } } while(false)                                                                                                                      


#define DEBUG(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Debug, std::source_location::current(), fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Warning, std::source_location::current(), fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Error,  std::source_location::current(), fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__)
