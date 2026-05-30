#pragma once 
#include "singleton.h"

// TODO: Load configs

namespace kubvc::application {
    enum class MathMode {
        Real, 
        Complex
    };

    class ApplicationConfig : public utility::Singleton<ApplicationConfig> {
        public: 
            ApplicationConfig() = default;
            ~ApplicationConfig() { }

            void setMode(MathMode mode) { m_mode = mode; }
            [[nodiscard]] MathMode getMode() const { return m_mode; }

        private:
            MathMode m_mode;
    };
}