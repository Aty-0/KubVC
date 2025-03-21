#include "renderer.h"
#include "logger.h"

#include "Libs/imgui/imgui.h"

namespace kubvc::render
{
    static constexpr glm::vec4 CLEAR_COLOR = { 0.0f, 0.0f, 0.0f, 1.0f };

    void Renderer::init()
    {
        auto initStatus = glewInit();
        if (initStatus != GLEW_OK)
        {
            const auto errStr = glewGetErrorString(initStatus);
            FATAL("GLEW initialization failed! %s", errStr);
        }

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
    }
}