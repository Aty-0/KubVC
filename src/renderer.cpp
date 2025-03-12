#include "renderer.h"
#include "logger.h"

#include "Libs/imgui/imgui.h"

namespace kubvc::render
{
    static constexpr glm::vec4 CLEAR_COLOR = { 0.16f, 0.08f, 0.2f, 1.0f };

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
        
        glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(Renderer::onGetError, nullptr);		
    }

    void Renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);

        // TODO: Imgui draw
    }

    void Renderer::onGetError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
	{
		const auto source_str = [source]() {
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		const auto type_str = [type]() {
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:  return "MARKER";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		const auto severity_str = [severity]() {
			switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "UNKNOWN";
			}
		}();

		static const bool verbose = false;
		
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			ERROR("GL: %s %s %s %s", source_str, type_str, severity_str, message);
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			WARN("GL: %s %s %s %s", source_str, type_str, severity_str, message);
			break;
		default:
			if (verbose)
			{
				DEBUG("GL: %s %s %s %s", source_str, type_str, severity_str, message);
			}
			break;
		}

	}
}