#include "gui.h"
#include "window.h"
#include "logger.h"

#include "Libs/imgui/imgui_impl_opengl3.h"
#include "Libs/imgui/imgui_impl_glfw.h"
#include "Libs/imgui/imgui_internal.h"
#include "Libs/imgui/implot_internal.h"

namespace kubvc::render
{
	static const auto DEFAULT_FONT_SIZE = 18.0f; 

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

		io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", DEFAULT_FONT_SIZE);
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
}