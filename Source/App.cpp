#include "StdAfx.h"
#include "App.h"

#include <GLFW/glfw3.h>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

struct ScopeContext
{
	ImGuiContext* NewContext;

	ImGuiContext* PrevContext;

	ScopeContext(ImGuiContext* newCtx)
		: NewContext(newCtx)
		, PrevContext(ImGui::GetCurrentContext())
	{
		if(NewContext != PrevContext)
		{
			ImGui::SetCurrentContext(NewContext);
		}
	}

	~ScopeContext()
	{
		if (NewContext != PrevContext)
		{
			ImGui::SetCurrentContext(PrevContext);
		}
	}

	operator bool() const
	{
		return NewContext != nullptr;
	}
};

App::App()
{
	CreateGLFWObjects();
	CreateImGui();
}

App::~App()
{
	if(Window)
	{
		glfwDestroyWindow(Window);
	}

	if (ScopeContext _ = ScopeContext(ImGuiCtx))
	{
		ImPlot::DestroyContext();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}

void App::Run(const TRenderCallback renderCallback)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwSwapInterval(1);

	auto clearDisplay = [this]()
	{
		static ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		int displayW, displayH;
		glfwGetFramebufferSize(Window, &displayW, &displayH);
		glViewport(0, 0, displayW, displayH);
		glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
	};

	if (ScopeContext _ = ScopeContext(ImGuiCtx))
	{
		while (!glfwWindowShouldClose(Window))
		{
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			renderCallback();

			ImGui::Render();

			clearDisplay();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(Window);
		}
	}
}

void App::CreateGLFWObjects()
{
	if(!glfwInit())
	{
		throw std::exception("Failed glfw init");
	}

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	const int width = static_cast<int>(mode->width * 0.70);
	const int height = static_cast<int>(mode->height * 0.70);

	Window = glfwCreateWindow(width, height, "App", nullptr, nullptr);
	if (!Window)
	{
		throw std::exception("Failed create window");
	}

	glfwMakeContextCurrent(Window);
}

void App::CreateImGui()
{
	IMGUI_CHECKVERSION();
	ImGuiCtx = ImGui::CreateContext();

	if (ScopeContext _ = ScopeContext(ImGuiCtx))
	{
		ImGui::StyleColorsDark();

		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 5.0f;
			style.PopupRounding = 3.0f;
		}

		if (ImGui_ImplGlfw_InitForOpenGL(Window, true) == false)
		{
			throw std::exception("Failed init ImGui with GLFW");
		}

		if (ImGui_ImplOpenGL3_Init("#version 150") == false)
		{
			throw std::exception("Failed init ImGui with OpenGL");
		}

		ImPlotCtx = ImPlot::CreateContext(); // TO-DO: Need Scope Context

		{
			ImPlot::StyleColorsAuto();

			ImPlotStyle& style = ImPlot::GetStyle();
			style.AntiAliasedLines = true;
		}
	}
}
