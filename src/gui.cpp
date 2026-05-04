#include "gui.h"
#include "window.h"
#include "logger.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "implot_internal.h"

#include "IconsFontAwesome6.h"

namespace kubvc::render {
	static constexpr auto DEFAULT_FONT_SIZE = 18.0f; 
	static constexpr auto MATH_FONT_SIZE = 22.0f; 

    void GUI::init() {
		KUB_DEBUG("Initialize Imgui...");
		auto window = kubvc::application::Window::getInstance();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		
		ImPlot::CreateContext();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    
		
		if (!ImGui_ImplGlfw_InitForOpenGL(&window->getHandle(), true)) {
			KUB_FATAL("ImGui GLFW impl failed!");
		}

		if (!ImGui_ImplOpenGL3_Init()) {
			KUB_FATAL("ImGui OpenGL3 init failed!");
		}	

		m_defaultFont = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Light.ttf", DEFAULT_FONT_SIZE);
		m_defaultFontMathSize = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Light.ttf", MATH_FONT_SIZE * 1.5f);
		m_mathFont = io.Fonts->AddFontFromFileTTF("fonts/OldStandard-Regular.ttf", MATH_FONT_SIZE);


		ImFontConfig config;
		config.MergeMode = true; 
		config.GlyphMinAdvanceX = DEFAULT_FONT_SIZE; 
		config.PixelSnapH = true;

		static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		m_iconFont = io.Fonts->AddFontFromFileTTF("fonts/fa-solid-900.ttf", DEFAULT_FONT_SIZE, &config, icon_ranges);

		applyDefaultKubDarkTheme();
    }

	void GUI::beginDockspace() {
		const auto viewport = ImGui::GetMainViewport();
	
		const auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoScrollbar;

		
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);			
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	
		ImGui::Begin("##dockspace_wnd", nullptr, windowFlags);

		const auto dockId = ImGui::GetID("dockspace");
		ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	}

	void GUI::endDockspace() {
		ImGui::PopStyleVar(3);
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
	
	// TODO: Save current theme to config

	void GUI::applyImGuiClassicTheme() {
		ImGui::StyleColorsClassic();
		ImPlot::StyleColorsClassic();
	}

	void GUI::applyImGuiWhiteTheme() {
		ImGui::StyleColorsLight();
		ImPlot::StyleColorsLight();
	}

    void GUI::applyImGuiDarkTheme() {
		ImGui::StyleColorsDark();
		ImPlot::StyleColorsDark();
	}

	void GUI::applyDefaultKubDarkTheme() {
		auto& style = ImGui::GetStyle();
		
		style.Colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);          	
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.56f, 0.56f, 0.60f, 1.00f);   
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.13f, 1.00f);       
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);        
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.24f, 0.24f, 0.26f, 0.50f);         
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);         
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);  
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.30f, 1.00f);   
		
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 0.70f);
		
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
		
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.90f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.32f, 0.80f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.33f, 0.38f, 0.90f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.38f, 0.38f, 0.44f, 1.00f);
		               		
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.62f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.50f, 0.80f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.62f, 0.90f, 1.00f);		
		style.Colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.68f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.55f, 0.85f, 1.00f);		
		style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.23f, 0.29f, 1.00f);        
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.32f, 0.42f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.38f, 0.50f, 1.00f);		
		style.Colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.68f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.35f, 0.55f, 0.85f, 1.00f);		
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.35f, 0.20f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.50f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.62f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.25f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.16f, 0.97f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.17f, 0.20f, 1.00f);		
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.66f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.40f, 0.62f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.55f, 0.65f, 0.80f, 1.00f);  
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.45f, 0.68f, 0.95f, 1.00f);		
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.42f, 0.68f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.40f, 0.62f, 0.90f, 0.80f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.40f, 0.62f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.90f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.60f);
		
		style.WindowRounding = 6.0f;
		style.ChildRounding = 6.0f;
		style.FrameRounding = 4.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 4.0f;
		
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.CellPadding = ImVec2(6.0f, 4.0f);
		style.ItemSpacing = ImVec2(10.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
		style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
		style.IndentSpacing = 22.0f;
		
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		
		style.ScrollbarSize = 14.0f;
		style.GrabMinSize = 12.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		
		auto& plotStyle = ImPlot::GetStyle();
		plotStyle.Colors[ImPlotCol_PlotBg] = ImVec4(0.11f, 0.11f, 0.13f, 1.00f);
		plotStyle.Colors[ImPlotCol_PlotBorder] = ImVec4(0.30f, 0.30f, 0.40f, 0.30f);
		plotStyle.Colors[ImPlotCol_Selection] = ImVec4(0.40f, 0.62f, 0.90f, 0.40f); 
		plotStyle.Colors[ImPlotCol_Crosshairs] = ImVec4(0.80f, 0.80f, 0.90f, 0.40f);
		
		plotStyle.PlotPadding = ImVec2(12.0f, 12.0f);
		plotStyle.PlotBorderSize = 1.0f;
		plotStyle.MinorAlpha = 0.20f;
		plotStyle.MajorTickLen = ImVec2(8.0f, 8.0f);
		plotStyle.MinorTickLen = ImVec2(4.0f, 4.0f);
		plotStyle.MajorTickSize = ImVec2(0.5f, 0.5f);  
		plotStyle.MinorTickSize = ImVec2(0.5f, 0.5f);
		plotStyle.MajorGridSize = ImVec2(0.5f, 0.5f);
		plotStyle.MinorGridSize = ImVec2(0.5f, 0.5f);
		
		style.AntiAliasedLines = true;
		
	}
}