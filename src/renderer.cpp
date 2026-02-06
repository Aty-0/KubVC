#include "renderer.h"
#include "logger.h"

#include "Libs/imgui/imgui.h"

namespace kubvc::render {
    static constexpr glm::vec4 CLEAR_COLOR = { 0.0f, 0.0f, 0.0f, 1.0f };

    auto Renderer::init() -> void {
        auto initStatus = glewInit();
        if (initStatus != GLEW_OK) {
            const auto errStr = glewGetErrorString(initStatus);
            KUB_FATAL("GLEW initialization failed! {}", errStr);
        }

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    auto Renderer::clear() -> void {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
    }
}