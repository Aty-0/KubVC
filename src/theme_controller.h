#pragma once 
#include "singleton.h"
#include "container.h"
#include "gui.h"
#include "io.h"
#include "logger.h"

#include <utility>
#include <variant>
#include <string>
#include <filesystem>

#include <magic_enum/magic_enum.hpp>

namespace kubvc::render::themes {
    using ImGuiColorScheme = ImGuiCol_;
    using ImPlotColorScheme = ImPlotCol_;

    static constexpr auto IMGUI_ENTRIES_SCHEME = magic_enum::enum_entries<ImGuiColorScheme>();
    static constexpr auto IMPLOT_ENTRIES_SCHEME = magic_enum::enum_entries<ImPlotColorScheme>();

    template <typename T>
    concept IsImStyle = std::is_same<T, ImGuiStyle>::value || std::is_same<T, ImPlotStyle>::value;
    
    using ImStyleValuesVariant = std::variant<float*, bool*, ImVec2*>;
    using ImStyleMiscEntries = std::vector<std::pair<std::string_view, ImStyleValuesVariant>>;
    
    using ImStyleSchemeVariant = std::variant<
        decltype(IMGUI_ENTRIES_SCHEME), 
        decltype(IMPLOT_ENTRIES_SCHEME)>;
    

    struct Theme {
        Theme() : name { "Untitled" }, data { }, index { 0 } { }
        explicit Theme(std::string_view name, std::size_t index) : name { name }, data { }, index { index } { }

        std::string name;
        korobok::krb data;
        std::size_t index;
    };

    class ThemeController : public utility::Singleton<ThemeController> {
        public:
            ThemeController();
            ~ThemeController() = default;
            
            void loadThemes();
            void setTheme(std::string_view name);

            // Load theme from file
            void load(std::string_view path); 
            // Save current theme 
            void save(std::string_view path, std::string_view themeName = "");

            void cacheMiscImGuiEntries(ImGuiStyle& style);
            void cacheMiscImPlotEntries(ImPlotStyle& style);
            
            [[nodiscard]] std::span<const std::unique_ptr<Theme>> getThemes() const;
            
        private:
            void applyCurrentTheme();

            void applyImGuiDarkTheme();
            void applyImGuiClassicTheme();
            void applyImGuiWhiteTheme();

            template <IsImStyle T>
            void loadColorScheme(const korobok::krb& data, T& style, const ImStyleSchemeVariant& entries);
            void loadMiscEntries(const korobok::krb& data, const ImStyleMiscEntries& entries);

            template <IsImStyle T>
            void saveColorScheme(korobok::krb& data, const T& style, const ImStyleSchemeVariant& entries);
            void saveMiscEntries(korobok::krb& data, const ImStyleMiscEntries& entries);

            ImStyleMiscEntries m_imguiMiscEntries;
            ImStyleMiscEntries m_implotMiscEntries;

            std::vector<std::unique_ptr<Theme>> m_themes;
            std::size_t m_currentThemeIndex;
    };

    static constexpr std::string_view IMGUI_DEFAULT_DARK_THEME = "ImGui Dark Theme"; 
    static constexpr std::string_view IMGUI_DEFAULT_WHITE_THEME = "ImGui White Theme"; 
    static constexpr std::string_view IMGUI_DEFAULT_CLASSIC_THEME = "ImGui Classic Theme"; 

    inline ThemeController::ThemeController() : 
        m_imguiMiscEntries { }, 
        m_implotMiscEntries { }, 
        m_themes { },
        m_currentThemeIndex { 0 } {
            // Add default imgui themes
            m_themes.push_back(std::move(std::make_unique<Theme>(IMGUI_DEFAULT_DARK_THEME, 0)));
            m_themes.push_back(std::move(std::make_unique<Theme>(IMGUI_DEFAULT_WHITE_THEME, 1)));
            m_themes.push_back(std::move(std::make_unique<Theme>(IMGUI_DEFAULT_CLASSIC_THEME, 2))); 

            loadThemes();
    }
    
    inline std::span<const std::unique_ptr<Theme>> ThemeController::getThemes() const {
        return m_themes;
    }

    inline void ThemeController::loadThemes() {
        static constexpr std::string_view THEMES_PATH = "themes"; // Actually just a folder name
        static constexpr std::string_view THEME_EXT = ".krb";
        
        if (std::filesystem::is_directory(THEMES_PATH)) {
            for (const auto& entry : std::filesystem::directory_iterator(THEMES_PATH)) {
                const auto& path = entry.path();
                if (path.extension() == THEME_EXT) {
                    load(path.relative_path().string());
                }
            }
        } else {
            KUB_WARN("themes directory is missing...");
        }
    }
    
    inline void ThemeController::setTheme(std::string_view name) {
        const auto& it = std::ranges::find_if(m_themes, [name](const auto& theme){ 
            return theme->name == name;
        });
        if (it == m_themes.end()) {
            KUB_ERROR("failed to find theme with name {}", name);
            return;
        }

        const auto& findTheme = *it;
        if (findTheme->index == m_currentThemeIndex) {
            return;
        }

        m_currentThemeIndex = findTheme->index;
        
        if (name == IMGUI_DEFAULT_CLASSIC_THEME) {
            applyImGuiClassicTheme();
        } else if (name == IMGUI_DEFAULT_DARK_THEME) {
            applyImGuiDarkTheme();
        } else if (name == IMGUI_DEFAULT_WHITE_THEME) {        
            applyImGuiWhiteTheme();
        } else {
            // Custom theme 
            applyCurrentTheme();
        }
    }

    inline void ThemeController::applyImGuiClassicTheme() {
		ImGui::StyleColorsClassic();
		ImPlot::StyleColorsClassic();
	}

	inline void ThemeController::applyImGuiWhiteTheme() {
		ImGui::StyleColorsLight();
		ImPlot::StyleColorsLight();
	}

    inline void ThemeController::applyImGuiDarkTheme() {
		ImGui::StyleColorsDark();
		ImPlot::StyleColorsDark();
	}

    template <IsImStyle T>
    inline void ThemeController::loadColorScheme(const korobok::krb& data, T& style, const ImStyleSchemeVariant& entries) {
        std::visit([&](const auto& entriesArray) {
            for (const auto& [entryValue, entryName] : entriesArray) {
                using SchemeType = decltype(entryValue);
                try {
                    const std::vector<float>& vec = data.at(entryName);
                    if (vec.size() == 4) {
                        const auto colorIndex = static_cast<SchemeType>(entryValue);
                        style.Colors[colorIndex] = ImVec4 { vec[0], vec[1], vec[2], vec[3] };
                    } else {
                        KUB_ERROR("ignore {} because it has wrong args count", entryName);
                    }
                } catch (const std::bad_variant_access& ex) {
                    KUB_ERROR("ignore {} because bad_variant_access", entryName);
                } catch (const std::invalid_argument& ex) {
                    KUB_ERROR("ignore {} because invalid_argument, can't find token", entryName);
                }
            }
        }, entries);
    }

    inline void ThemeController::loadMiscEntries(const korobok::krb& data, const ImStyleMiscEntries& entries) {
        for (const auto& [entryName, entryValue] : entries) {  
            try {
                if (const auto* ptr = std::get_if<float*>(&entryValue)) {
                    **ptr = data.at(entryName);
                } else if (const auto* ptr = std::get_if<bool*>(&entryValue)) {
                    **ptr = data.at(entryName);
                } else if (const auto* ptr = std::get_if<ImVec2*>(&entryValue)) {
                    const std::vector<float>& vec = data.at(entryName);
                    **ptr = { vec[0], vec[1] };
                } else {
                    KUB_ERROR("failed to load {} missing type", entryName);
                }
            } catch (const std::invalid_argument& ex) {
                KUB_ERROR("ignore {} because invalid_argument, can't find token!", entryName);
            }
        }
    }
    
    inline void ThemeController::applyCurrentTheme() {
        if (m_currentThemeIndex > m_themes.size()) {
            KUB_ERROR("can't apply current theme because current theme index > themes table size");
            return;
        }

        const auto& currentTheme = m_themes[m_currentThemeIndex];

        auto& imguiStyle = ImGui::GetStyle();
        loadColorScheme(currentTheme->data, imguiStyle, IMGUI_ENTRIES_SCHEME);

        auto& implotStyle = ImPlot::GetStyle();
        loadColorScheme(currentTheme->data, implotStyle, IMPLOT_ENTRIES_SCHEME);

        loadMiscEntries(currentTheme->data, m_imguiMiscEntries);
        loadMiscEntries(currentTheme->data, m_implotMiscEntries);  
    }

    inline void ThemeController::load(std::string_view path) {
        KUB_DEBUG("load theme {}", path);

        io::FileLoader loader { };
        const auto& result = loader.load(path);
        if (!result.has_value()) {
            KUB_ERROR("failed to theme load file");
            return;
        }

        auto theme = std::make_unique<Theme>();
        theme->index = m_themes.size();
        if (!theme->data.from(result.value()).has_value()) {
            KUB_ERROR("failed to parse theme file data");
            return;
        }
        try {
            theme->name = theme->data.at("ThemeName");      
        } catch (const std::invalid_argument& ex) {
            KUB_ERROR("failed to get theme name.");
        }

        m_themes.push_back(std::move(theme));
    }

    template <IsImStyle T>
    inline void ThemeController::saveColorScheme(korobok::krb& data, const T& style, const ImStyleSchemeVariant& entries) {
        std::visit([&](const auto& entriesArray) {
            for (const auto& [entryValue, entryName] : entriesArray) {
                using SchemeType = decltype(entryValue);
                const auto colorIndex = static_cast<SchemeType>(entryValue);
                const auto& colorVector = style.Colors[colorIndex];
                data[entryName] = { 
                    colorVector.x, 
                    colorVector.y, 
                    colorVector.z, 
                    colorVector.w 
                };
            }
        }, entries);
    }

    inline void ThemeController::saveMiscEntries(korobok::krb& data, const ImStyleMiscEntries& entries) {
        for (const auto& [entryName, entryValue] : entries) {  
            if (const auto* ptr = std::get_if<float*>(&entryValue)) {
                data[entryName] = **ptr;
            } else if (const auto* ptr = std::get_if<bool*>(&entryValue)) {
                data[entryName] = **ptr;
            } else if (const auto* ptr = std::get_if<ImVec2*>(&entryValue)) {
                const auto vec2 = **ptr;
                data[entryName] = { vec2.x, vec2.y };
            } else {
                KUB_ERROR("failed to save {} missing type", entryName);
            }
        }
    }

    inline void ThemeController::save(std::string_view path, std::string_view themeName) {
        if (m_currentThemeIndex > m_themes.size()) {
            KUB_ERROR("can't apply current theme because current theme index > themes table size");
            return;
        }

        const auto& currentTheme = m_themes[m_currentThemeIndex];
        auto& data = currentTheme->data;
        static constexpr std::string_view THEME_NAME_TOKEN = "ThemeName";
        const auto& it = std::ranges::find_if(data.tokens(), [](const auto& token) { return token.name() == THEME_NAME_TOKEN; });
        if (it == data.tokens().end()) {
            if (themeName.empty()) {
                KUB_ERROR("Failed to get theme name. Set name as untitled!");
                data[THEME_NAME_TOKEN] = "Untitled";
            } else {
                data[THEME_NAME_TOKEN] = themeName;
            }
        }

        const auto& imguiStyle = ImGui::GetStyle();
        saveColorScheme(data, imguiStyle, IMGUI_ENTRIES_SCHEME);
        saveMiscEntries(data, m_imguiMiscEntries);
            
        // TODO: korobok: group create
        const auto& implotStyle = ImPlot::GetStyle();
        saveColorScheme(data, implotStyle, IMPLOT_ENTRIES_SCHEME);
        saveMiscEntries(data, m_implotMiscEntries);

        const auto& dump = data.dump();
        io::FileSaver saver { };
        saver.save(path, std::vector<char> { dump.begin(), dump.end() });
    }

    inline void ThemeController::cacheMiscImPlotEntries(ImPlotStyle& style) {
        KUB_ASSERT(m_implotMiscEntries.empty(), "Trying to cache misc entries when vector is not empty");
        m_implotMiscEntries = {
		    { "PlotPadding", &style.PlotPadding },
		    { "PlotBorderSize", &style.PlotBorderSize },  
		    { "MinorAlpha", &style.MinorAlpha },      
		    { "MajorTickLen", &style.MajorTickLen },  
		    { "MinorTickLen", &style.MinorTickLen },  
		    { "MajorTickSize", &style.MajorTickSize },   
		    { "MinorTickSize", &style.MinorTickSize }, 
		    { "MajorGridSize", &style.MajorGridSize }, 
		    { "MinorGridSize", &style.MinorGridSize }, 
        };
    }

    inline void ThemeController::cacheMiscImGuiEntries(ImGuiStyle& style) {
        KUB_ASSERT(m_imguiMiscEntries.empty(), "Trying to cache misc entries when vector is not empty");
        m_imguiMiscEntries = {
            { "WindowRounding", &style.WindowRounding  },
            { "ChildRounding", &style.ChildRounding },
            { "FrameRounding", &style.FrameRounding },
            { "PopupRounding", &style.PopupRounding },
            { "ScrollbarRounding", &style.ScrollbarRounding }, 
            { "GrabRounding", &style.GrabRounding },
            { "TabRounding" , &style.TabRounding },
            { "WindowPadding", &style.WindowPadding  },
            { "FramePadding", &style.FramePadding   },
            { "CellPadding", &style.CellPadding    },
            { "ItemSpacing", &style.ItemSpacing    },
            { "ItemInnerSpacing", &style.ItemInnerSpacing  }, 
            { "TouchExtraPadding", &style.TouchExtraPadding }, 
            { "IndentSpacing", &style.IndentSpacing     }, 
            { "WindowBorderSize", &style.WindowBorderSize },
            { "ChildBorderSize", &style.ChildBorderSize },
            { "PopupBorderSize", &style.PopupBorderSize },
            { "FrameBorderSize", &style.FrameBorderSize },
            { "TabBorderSize", &style.TabBorderSize },		
            { "ScrollbarSize", &style.ScrollbarSize  },
            { "GrabMinSize", &style.GrabMinSize    },
            { "WindowTitleAlign", &style.WindowTitleAlign  },
            { "AntiAliasedLines", &style.AntiAliasedLines  },
        };
    }
}