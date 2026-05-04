#pragma once
#include <fstream>
#include <optional>
#include <vector>

#include "logger.h"

namespace kubvc::io {
    class FileSaver {
        public:
            FileSaver() = default;
            explicit FileSaver(std::string_view path, const std::vector<char>& buffer);

            bool save(std::string_view path, const std::vector<char>& buffer);

    };

    FileSaver::FileSaver(std::string_view path, const std::vector<char>& buffer) {
        KUB_ASSERT(save(path, buffer), "Can't save file from contructor, see logs");
    } 
    
    bool FileSaver::save(std::string_view path, const std::vector<char>& buffer) {
        if (path.empty()) {
            KUB_ERROR("Can't save file because path is empty");
            return false;
        }
        std::ofstream file;
        file.open(path.data(), std::ios_base::out);
        if (!file.is_open()) {
            return false;
        }

        file.write(buffer.data(), buffer.size());
        file.close();

        return true;
    }

    class FileLoader {
        public:
            FileLoader() = default;
            FileLoader(std::string_view path);
            ~FileLoader();

            std::optional<std::string> load(std::string_view path);

            [[nodiscard]] const std::vector<char> getBuffer() const { return m_buffer; }

        private:
            std::vector<char> m_buffer;     
    };

    inline FileLoader::FileLoader(std::string_view path) {
        const auto result = load(path);
        KUB_ASSERT(result.has_value(), "Can't load file from contructor, see logs");
    }

    inline FileLoader::~FileLoader() {
        m_buffer.clear();
        m_buffer.shrink_to_fit();
    }

    inline std::optional<std::string> FileLoader::load(std::string_view path) {
        if (path.empty()) {
            KUB_ERROR("Path is empty");
            return std::nullopt;
        }
        std::ifstream file = std::ifstream(path.data(), std::iostream::ate | std::ios::binary);
        if (!file.is_open()) {
            // TODO: details
            KUB_ERROR("failed to open file");
            return std::nullopt;
        }
        
        if (!m_buffer.empty()) {
            m_buffer.clear();
            m_buffer.shrink_to_fit();
        }

        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        m_buffer.resize(size);
        if (!file.read(m_buffer.data(), size)) {
            // TODO: details
            KUB_ERROR("failed to read file");
            file.close();
            return std::nullopt;
        }

        const auto data = std::string(m_buffer.begin(), m_buffer.end());
        file.close();

        return data;
    }

}