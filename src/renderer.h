#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>

#include "singleton.h"

namespace kubvc::render
{
    class Renderer : public utility::Singleton<Renderer> 
    {
        public:
            void init();
            void render();            
        private:
            static void onGetError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
    };
}