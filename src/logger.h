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

            auto print(const Logger::LogLevel& level, const std::source_location source, std::string_view text) -> void;   

            template<typename... Args>
            auto print(const Logger::LogLevel& level, const std::source_location source, std::string_view text, Args&&... args) -> void const;        

            auto save(std::string_view path) -> void;

        private:
            std::ofstream m_file;     
            std::basic_stringbuf<char> m_buffer;        
    };
    
    template<typename... Args>
    inline auto Logger::print(const Logger::LogLevel& level, const std::source_location source, 
        std::string_view text, Args&&... args) -> void const {
        auto format_args = std::make_format_args(
            [](auto&& arg) -> decltype(auto) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, const unsigned char*> || std::is_same_v<T, unsigned char*>) {
                    return reinterpret_cast<const char*>(arg);
                } 
                else {
                    return std::forward<decltype(arg)>(arg);
                }
            }(args)...
        );

        auto mergedArgsText = std::vformat(text, format_args);
        print(level, source, mergedArgsText);
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
