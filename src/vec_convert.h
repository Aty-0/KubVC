#pragma once 
#include "libs/imgui/imgui.h"
#include <glm/glm.hpp>

namespace kubvc::utility {
    static inline auto toImVec4(glm::vec4 vec) -> ImVec4 {
        return ImVec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toImVec2(glm::vec2 vec) -> ImVec2 {
        return ImVec2(vec.x, vec.y);
    }

    static inline auto toGlmVec4(ImVec4 vec) -> glm::vec4 {
        return glm::vec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toGlmVec2(ImVec2 vec) -> glm::vec2 {
        return glm::vec2(vec.x, vec.y);
    }
}