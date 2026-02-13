#pragma once 
#include "libs/imgui/imgui.h"
#include <glm/glm.hpp>

namespace kubvc::utility {
    static inline ImVec4 toImVec4(glm::vec4 vec) {
        return ImVec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline ImVec2 toImVec2(glm::vec2 vec) {
        return ImVec2(vec.x, vec.y);
    }

    static inline glm::vec4 toGlmVec4(ImVec4 vec) {
        return glm::vec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline glm::vec2 toGlmVec2(ImVec2 vec) {
        return glm::vec2(vec.x, vec.y);
    }
}