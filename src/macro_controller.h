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
        explicit Macro(std::string_view name, std::string_view value, std::int32_t id) : name { name }, value { value }, m_id { id } { }

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
        
        std::unique_lock lock { m_mutex };
        
        io::FileSaver saver;
        std::vector<char> buffer;        
        for (const auto& macro : m_macros) {
            if (macro.name.empty() || macro.value.empty()) {
                KUB_WARN("macro save: name or value is empty!");
                continue;
            }

            const auto fmt = std::format("{}:\"{}\"\n", macro.name, macro.value);
            buffer.insert(buffer.end(), fmt.begin(), fmt.end());
        }

        if (!saver.save(path, buffer)) {
            KUB_ERROR("failed to save macros list");
        }
    }

    inline void MacroController::load(std::string_view path) {
        std::unique_lock lock { m_mutex };

        io::FileLoader loader;
        const auto& result = loader.load(path);
        if (result.has_value()) {
            korobok::krb data { };
            const auto& tokens = data.from(result.value());
            if (!tokens.has_value()) {
                KUB_ERROR("failed to parse macros file");
                return;
            }

            for (const auto& token : tokens.value()) {
                const auto& value = token.value<std::string>();
                if (!value.has_value()) {
                    KUB_ERROR("failed to get value in {} token, possible wrong type of token", token.name());
                    continue;
                }

                m_macros.push_back(std::move(Macro { 
                    token.name(),
                    value.value().get(),
                    (++m_globalId)
                }));    
            }
     
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