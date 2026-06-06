#pragma once 
#include "singleton.h"
#include "io.h"
#include "alg_helpers.h"

#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <shared_mutex>
#include <mutex>
#include <ranges>
#include <regex>
#include <optional>

namespace kubvc::algorithm {
    struct Macro {
        std::string name;
        std::string value;

        [[nodiscard]] std::int32_t getId() const { return m_id; }
        
        private: 
            friend class MacroController;
            
            std::int32_t m_id;
    };

    class MacroController : public utility::Singleton<MacroController> {
        public:
            MacroController();
            ~MacroController() = default;

            bool add(Macro&& macro);
            void remove(std::int32_t id);
            void load(std::string_view path);
            void save(std::string_view path);
            void appendMacrosToText(std::string& text);

            [[nodiscard]] std::optional<std::reference_wrapper<Macro>> getMacro(std::int32_t id);
            [[nodiscard]] std::vector<Macro> getMacros() const;

        private:
            mutable std::shared_mutex m_mutex;        
            std::vector<Macro> m_macros;
            std::atomic<std::int32_t> m_globalId;
    };

    inline MacroController::MacroController() : m_macros{ }, m_globalId{ 0 } {

    } 

    inline void MacroController::appendMacrosToText(std::string& text) {
        if (text.empty()) {
            return;
        }

        std::unique_lock lock(m_mutex);
        for (const auto& macro : m_macros) {
            // Find keyname in text output then replace it 
            const auto findName = std::regex("\\b" + macro.name + "\\b");            
            if (std::regex_search(text, findName)) {
                KUB_DEBUG("MacroController: replace {} to {}", macro.name, macro.value);
                text = std::regex_replace(text, std::regex(macro.name), macro.value);
            }
        }
    }

    inline void MacroController::save(std::string_view path) {
        if (m_macros.empty()) {            
            return;
        }

        io::FileSaver saver;
        std::vector<char> buffer;        
        for (const auto& macro : m_macros) {
            if (macro.name.empty() || macro.value.empty()) {
                KUB_WARN("macro save: name or value is empty!");
                continue;
            }

            const auto fmt = std::format("{}:{}\n", macro.name, macro.value);
            buffer.insert(buffer.end(), fmt.begin(), fmt.end());
        }

        if (!saver.save(path, buffer)) {
            KUB_ERROR("failed to save macros list");
        }
    }

    inline void MacroController::load(std::string_view path) {
        io::FileLoader loader;
        const auto& result = loader.load(path);
        if (result.has_value()) {
            const auto& value = result.value();
           
            auto parseLine = [this](const auto& range) -> std::optional<Macro> {
                auto splitValues = range | std::views::split(':') | std::views::transform([](const auto& sub) { 
                    return std::string(sub.begin(), sub.end());
                });
                
                std::vector<std::string> values { };
                for (const auto& sub : splitValues) {
                    values.emplace_back(sub.begin(), sub.end());
                }

                if (values.size() != 2) {
                    KUB_ERROR("macro load: wrong size of split, size:{}", values.size());
                    return std::nullopt;
                }

                if (values[0].empty() || values[1].empty()) {
                    KUB_ERROR("macro load: value 0 or 1 is empty!");
                    return std::nullopt;
                }

                auto macro = Macro { };
                macro.name = values[0];
                macro.value = values[1];
                macro.m_id = (++m_globalId);
                return macro;
            };
            
            auto parsed = value 
                | std::views::split('\n') // Split line by /n 
                | std::views::filter([](const auto& line) { 
                    return std::ranges::any_of(line, [](char c) { 
                        return algorithm::Helpers::isWhiteSpace(static_cast<algorithm::Helpers::uchar>(c)); 
                    });
                }) // Remove all whitespaces or special symbols 
                | std::views::transform(parseLine)  // Parse lines and create macros
                | std::views::filter([](const auto& result) { return result.has_value(); }) // Remove invalid lines
                | std::views::transform([](const auto& result) { return result.value(); }); // Make range with macros 
            // Save macros 
            m_macros = std::move(std::vector<Macro>(parsed.begin(), parsed.end()));         
        } else {
            KUB_ERROR("failed to open macros list file");
        }
    }

    inline bool MacroController::add(Macro&& macro) {
        std::unique_lock lock { m_mutex };
        if (macro.name.empty()) {
            return false;
        }

        // Check macro has unique name  
        const auto it = std::ranges::find_if(m_macros, [&macro](const auto& listMacro) 
            { return listMacro.name == macro.name; });
        
        if (it == m_macros.end()) {
            macro.m_id = (++m_globalId);
            m_macros.push_back(std::move(macro));
            return true;
        }

        return false;
    }

    inline void MacroController::remove(std::int32_t id) {
        std::unique_lock lock { m_mutex }; 
        const auto it = std::ranges::remove_if(m_macros, [id](const auto& macro) { return macro.m_id == id; });
        m_macros.erase(it.begin(), it.end());
    }

    inline std::vector<Macro> MacroController::getMacros() const {
        std::shared_lock lock { m_mutex };
        return m_macros;
    }

    inline std::optional<std::reference_wrapper<Macro>> MacroController::getMacro(std::int32_t id) {
        const auto it = std::ranges::find_if(m_macros, [id](const auto& macro) { return macro.m_id == id; });
        if (it != m_macros.end()) {
            return *it;
        }

        return std::nullopt; 
    }

}