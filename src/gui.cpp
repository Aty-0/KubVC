#include "gui.h"
#include "window.h"
#include "logger.h"

#include "Libs/imgui/imgui_impl_opengl3.h"
#include "Libs/imgui/imgui_impl_glfw.h"
#include "Libs/imgui/imgui_internal.h"
#include "Libs/imgui/implot_internal.h"

namespace kubvc::render
{
	static const auto DEFAULT_FONT_SIZE = 16.0f; 
	static const auto MATH_FONT_SIZE = 20.0f; 

    void GUI::init()
    {
		DEBUG("Initialize Imgui...");
		auto window = kubvc::application::Window::getInstance();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		
		ImPlot::CreateContext();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    
		
		if (!ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true))
		{
			FATAL("ImGui GLFW impl failed!");
		}

		if (!ImGui_ImplOpenGL3_Init())
		{
			FATAL("ImGui OpenGL3 init failed!");
		}	

		m_defaultFont = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Light.ttf", DEFAULT_FONT_SIZE);
		m_defaultFontMathSize = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Light.ttf", MATH_FONT_SIZE);
		m_mathFont = io.Fonts->AddFontFromFileTTF("fonts/OldStandard-Regular.ttf", MATH_FONT_SIZE);
    }

	void GUI::beginDockspace()
	{
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

		applyColorTheme();
	}

	void GUI::endDockspace()
	{
		ImGui::PopStyleVar(3);
		ImGui::End();
	}

    void GUI::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
    }
	
    void GUI::end()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::destroy()
    {
		ImPlot::DestroyContext();

        ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
    }
	
    void GUI::applyColorTheme()
	{
		auto& style = ImGui::GetStyle();
    	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    	style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    	style.Colors[ImGuiCol_Separator] = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);
    	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 0.20f);
    	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.67f);
    	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.95f);
    	style.Colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    	style.Colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		style.WindowRounding = 4.0f;
    	style.ChildRounding = 4.0f;
    	style.FrameRounding = 4.0f;
    	style.PopupRounding = 4.0f;
    	style.ScrollbarRounding = 4.0f;
    	style.GrabRounding = 4.0f;
    	style.TabRounding = 4.0f;
	
    	style.WindowPadding = ImVec2(8.0f, 8.0f);
    	style.FramePadding = ImVec2(6.0f, 4.0f);
    	style.CellPadding = ImVec2(4.0f, 2.0f);
    	style.ItemSpacing = ImVec2(8.0f, 4.0f);
    	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    	style.IndentSpacing = 20.0f;
	
    	style.WindowBorderSize = 1.0f;
    	style.ChildBorderSize = 1.0f;
    	style.PopupBorderSize = 1.0f;
    	style.FrameBorderSize = 1.0f;
    	style.TabBorderSize = 1.0f;
	
    	style.ScrollbarSize = 12.0f;
    	style.GrabMinSize = 10.0f;
    	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    	auto& plotStyle = ImPlot::GetStyle();
    	plotStyle.Colors[ImPlotCol_Fill] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    	plotStyle.Colors[ImPlotCol_MarkerOutline] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    	plotStyle.Colors[ImPlotCol_MarkerFill] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    	plotStyle.Colors[ImPlotCol_PlotBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    	plotStyle.Colors[ImPlotCol_PlotBorder] = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);
    	plotStyle.Colors[ImPlotCol_Selection] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    	plotStyle.Colors[ImPlotCol_Crosshairs] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);

		plotStyle.PlotPadding = ImVec2(10.0f, 10.0f);
    	plotStyle.LineWeight = 2.0f;
    	plotStyle.MarkerSize = 6.0f;
    	plotStyle.PlotBorderSize = 1.0f;
    	plotStyle.MinorAlpha = 0.25f;
    	plotStyle.MajorTickLen = ImVec2(10.0f, 10.0f);
    	plotStyle.MinorTickLen = ImVec2(5.0f, 5.0f);
    	plotStyle.MajorTickSize = ImVec2(1.0f, 1.0f);
    	plotStyle.MinorTickSize = ImVec2(1.0f, 1.0f);
    	plotStyle.MajorGridSize = ImVec2(1.0f, 1.0f);
    	plotStyle.MinorGridSize = ImVec2(1.0f, 1.0f);

	}
}