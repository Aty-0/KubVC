#include "gui.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <implot_internal.h>
#include <IconsFontAwesome6.h>

#include "../application/window.h"
#include "../utility/logger.h"
#include "theme_controller.h"

namespace kubvc::render {
	static constexpr auto DEFAULT_FONT_SIZE = 18.0f; 
	static constexpr auto MATH_FONT_SIZE = 22.0f; 
	static constexpr auto DEFAULT_CONFIG_FLAGS = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
	static constexpr auto DOCKSPACE_WINDOW_FLAGS = ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoScrollbar;

    void GUI::init() {
		const auto window = kubvc::application::Window::getInstance();
		KUB_DEBUG("Initialize Imgui...");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();

		auto& io = ImGui::GetIO();
		io.ConfigFlags = DEFAULT_CONFIG_FLAGS;    
		
		KUB_ASSERT(ImGui_ImplGlfw_InitForOpenGL(&window->getHandle(), true), "ImGui GLFW impl failed!");
		KUB_ASSERT(ImGui_ImplOpenGL3_Init(), "ImGui OpenGL3 init failed!");

		m_defaultFont = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", DEFAULT_FONT_SIZE);
		m_defaultFontMathSize = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", MATH_FONT_SIZE * 1.5f);
		m_mathFont = io.Fonts->AddFontFromFileTTF("fonts/OldStandard-Regular.ttf", MATH_FONT_SIZE);

		ImFontConfig config{};
		config.MergeMode = true; 
		config.GlyphMinAdvanceX = DEFAULT_FONT_SIZE; 
		config.PixelSnapH = true;

		static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		m_iconFont = io.Fonts->AddFontFromFileTTF("fonts/fa-solid-900.ttf", DEFAULT_FONT_SIZE, &config, icon_ranges);

		setupThemeController();
    }

	void GUI::beginDockspaceWindow() {
		const auto viewport = ImGui::GetMainViewport();
		
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);			
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);	
		ImGui::Begin("##dockspace_wnd", nullptr, DOCKSPACE_WINDOW_FLAGS);
	}
	
	void GUI::dockspace() {
		const auto dockId = ImGui::GetID("dockspace");
		ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar(3);
	}

	void GUI::endDockspaceWindow() {
		ImGui::End();
	}

    void GUI::begin() {
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
    }
	
    void GUI::end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::destroy() {
		ImPlot::DestroyContext();

        ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
    }

	void GUI::setupThemeController() {
		static const auto themeController = themes::ThemeController::getInstance();
		themeController->cacheMiscImGuiEntries(ImGui::GetStyle());
		themeController->cacheMiscImPlotEntries(ImPlot::GetStyle());
		themeController->setTheme(themes::IMGUI_DEFAULT_DARK_THEME);
	}
}