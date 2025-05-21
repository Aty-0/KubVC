#pragma once 
#include "libs/imgui/imgui.h"
#include <glm/glm.hpp>

namespace kubvc::utility
{
    static inline auto toImVec4(glm::vec4 vec)
    {
        return ImVec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toImVec2(glm::vec2 vec)
    {
        return ImVec2(vec.x, vec.y);
    }

    static inline auto toGlmVec4(ImVec4 vec)
    {
        return glm::vec4(vec.x, vec.y, vec.z, vec.w);
    }

    static inline auto toGlmVec2(ImVec2 vec)
    {
        return glm::vec2(vec.x, vec.y);
    }
}