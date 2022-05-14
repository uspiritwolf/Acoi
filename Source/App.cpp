#include "Core.h"
#include "App.h"

#include <GLFW/glfw3.h>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

App* App::s_instance = nullptr;

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

struct ScopeFpsLock
{
	const double LockFrameTime;

	double FrameTime = 0.0;
	double CurrentTime = 0.0;
	double IdleTime = 0.0;

	bool MustRender = false;

	ScopeFpsLock(double lockFrameTime)
		: LockFrameTime(lockFrameTime)
	{

	}

	void UpdateTime(double time)
	{
		if (CurrentTime > 0.0)
		{
			const double newFrameTime = time - CurrentTime;
			if (newFrameTime >= LockFrameTime)
			{
				FrameTime = newFrameTime;
				CurrentTime = time;
				MustRender = true;
			}
			else
			{
				IdleTime = newFrameTime;
				MustRender = false;
			}
		}
		else
		{
			CurrentTime = time;
			FrameTime = LockFrameTime;
			MustRender = true;
		}
	}

	operator bool() const 
	{
		return MustRender;
	}
};

App::App()
{
	if (s_instance)
		throw std::exception("There can only be one app!");

	s_instance = this;

	CreateGLFWObjects();
	CreateImGui();
}

App::~App()
{
	s_instance = nullptr;

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

void App::Run(TUserRenderCallback userRenderCallback)
{
	UserRenderCallback = std::move(userRenderCallback);

	const auto windowUpdateCallback = [](GLFWwindow* window, int _, int __)
		{
			App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
			app->UpdateWindow();
		};

	glfwSetWindowUserPointer(Window, this);
	glfwSetWindowPosCallback(Window, windowUpdateCallback);
	glfwSetWindowSizeCallback(Window, windowUpdateCallback);

	if (ScopeContext _ = ScopeContext(ImGuiCtx))
	{
		while (!glfwWindowShouldClose(Window))
		{
			glfwPollEvents();
			UpdateWindow();
		}
	}

	glfwSetWindowPosCallback(Window, nullptr);
	glfwSetWindowUserPointer(Window, nullptr);
}

void App::UpdateWindow()
{
	FpsLock->UpdateTime(glfwGetTime());

	if (*FpsLock)
	{
		if(!Tasks.empty())
		{
			TaskMutex.lock();
			const auto tasks = std::move(Tasks);
			TaskMutex.unlock();
			for(auto& task : tasks)
			{
				task();
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		UserRenderCallback();

		ImGui::Render();

		ClearDisplay();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window);
	}
}

void App::ClearDisplay() const
{
	static ImVec4 clearColor = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	int displayW, displayH;
	glfwGetFramebufferSize(Window, &displayW, &displayH);
	glViewport(0, 0, displayW, displayH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	
	Window = glfwCreateWindow(width, height, "VoA", nullptr, nullptr);
	if (!Window)
	{
		throw std::exception("Failed create window");
	}

	glfwMakeContextCurrent(Window);

	glfwSwapInterval(0);

	FpsLock = std::make_unique<ScopeFpsLock>(1.0 / mode->refreshRate);
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

void App::DispatchInMainThread(const std::function<void()>& task)
{
	TaskMutex.lock();
	Tasks.emplace_back(task);
	TaskMutex.unlock();
}