#pragma once
#include <vector>
#include <string>
#include <optional>

namespace kubvc::utility {
    template<typename T>
    class Container {
        public:
            using iterator = typename std::vector<std::pair<std::string, T>>::iterator;
            using const_iterator = typename std::vector<std::pair<std::string, T>>::const_iterator;
            using reverse_iterator = typename std::vector<std::pair<std::string, T>>::reverse_iterator;
            using const_reverse_iterator = typename std::vector<std::pair<std::string, T>>::const_reverse_iterator;

            Container(const std::vector<std::pair<std::string, T>>& vec) : m_data(vec) { }
            
            inline std::optional<T> get(std::string_view name) const {
                const auto result = std::find_if(m_data.begin(), m_data.end(), [name](std::pair<std::string, T> it) {
                    return it.first == name;
                });

                if (result == m_data.end()) {
                    return std::nullopt;
                }

                return result->second;
            }

            inline bool find(std::string_view name) const {
                return std::find_if(m_data.begin(), m_data.end(), [name](std::pair<std::string, T> it) {
                    return it.first == name;
                }) != m_data.end();
            }

            inline std::vector<std::pair<std::string, T>> getData() const { return m_data; }
            
            inline iterator begin() { return m_data.begin(); }
            inline iterator end() { return m_data.end(); }
            inline reverse_iterator rbegin() { return m_data.rbegin(); }
            inline reverse_iterator rend() { return m_data.rend(); }
            inline const_iterator begin()  const { return m_data.begin(); }
            inline const_iterator end()  const { return m_data.end(); }
            inline const_reverse_iterator rbegin()  const { return m_data.rbegin(); }
            inline const_reverse_iterator rend()  const { return m_data.rend(); }

            inline const_iterator cbegin()  const { return m_data.cbegin(); }
            inline const_iterator cend()  const { return m_data.cend(); }
            inline const_reverse_iterator crbegin()  const { return m_data.crbegin(); }
            inline const_reverse_iterator crend()  const { return m_data.crend(); }
            
        private:
            std::vector<std::pair<std::string, T>> m_data;
    };
}