#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>

#include "singleton.h"

namespace kubvc::render {
    class Renderer : public utility::Singleton<Renderer> {
        public:
            void init();
            void clear();            
    };
}