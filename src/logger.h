#pragma once
#include "singleton.h"
#include <source_location>
#include <format>
#include <fstream>
#include <sstream>

namespace kubvc::utility {
    class Logger : public Singleton<Logger> {
        public:
            enum class LogLevel {
                Debug = 0,
                Warning,
                Error, 
                Fatal
            };

            ~Logger();


            template<typename... Args>
            void print(Logger::LogLevel level, const std::source_location source, std::string_view text, Args&&... args);        
            void print(Logger::LogLevel level, const std::source_location source, std::format_string<Args...> fmt, Args&&... args);        

            void save(std::string_view path);

        private:
            void printImpl(Logger::LogLevel level, const std::source_location source, std::string_view text);   
            
            std::ofstream m_file;     
            std::basic_stringbuf<char> m_buffer;        
    };
    
    template<typename... Args>
    inline void Logger::print(Logger::LogLevel level, const std::source_location source, 
        std::format_string<Args...> fmt, Args&&... args) {        
        printImpl(level, source, std::format(fmt, std::forward<Args>(args)...));
    }

    static inline auto log = kubvc::utility::Logger::getInstance();
}

// TODO: Rework
#define KUB_ASSERT(cond, fmt, ...) do {                                                                                                   \
        if (!(cond))                                                                                                                    \
        {                                                                                                                             \
            KUB_ERROR("Assertion failed: ");                                                                                              \
            kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__); \
        } } while(false)                                                                                                                      



#define KUB_EXPLICIT_DEF_DEBUG
#if defined(DEBUG) || defined(_DEBUG) || defined(KUB_EXPLICIT_DEF_DEBUG)
    #define KUB_IS_DEBUG 
#endif

#ifdef KUB_IS_DEBUG
    #define KUB_DEBUG(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Debug, std::source_location::current(), fmt, ##__VA_ARGS__)
#else 
    #define KUB_DEBUG(fmt, ...) 
#endif

#define KUB_WARN(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Warning, std::source_location::current(), fmt, ##__VA_ARGS__)
#define KUB_ERROR(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Error,  std::source_location::current(), fmt, ##__VA_ARGS__)
#define KUB_FATAL(fmt, ...) kubvc::utility::log->print(kubvc::utility::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__)
