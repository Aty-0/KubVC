#include "gui.h"
#include "window.h"
#include "logger.h"

#include "Libs/imgui/imgui_impl_opengl3.h"
#include "Libs/imgui/imgui_impl_glfw.h"
#include "Libs/imgui/imgui_internal.h"

#include "Libs/imgui/implot.h"
#include "Libs/imgui/implot_internal.h"

namespace kubvc::render
{
	static constexpr int test_data[] = { 0,1,2,3 };
	void GUIPlot::body()
	{
        if (ImPlot::BeginPlot(m_name.c_str())) {
			inBody();

			ImPlot::PlotLine("My Line Plot", test_data, test_data, 4);
            ImPlot::EndPlot();
        }
    } 

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
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    
		
		if (!ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true))
		{
			FATAL("ImGui GLFW impl failed!");
		}

		if (!ImGui_ImplOpenGL3_Init())
		{
			FATAL("ImGui OpenGL3 init failed!");
		}

		// TODO: 
		m_elements.push_back(new GUIPlot());
    }

    void GUI::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
    }
	
	void GUI::draw()
	{		
		for (auto element : m_elements)
		{
			element->body();
		}
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