#pragma once

struct GLFWwindow;
struct ImGuiContext;
struct ImPlotContext;

class App
{
public:
	using TUserRenderCallback = std::function<void()>;

private:

	static App* s_instance;

	GLFWwindow* Window = nullptr;

	ImGuiContext* ImGuiCtx = nullptr;

	ImPlotContext* ImPlotCtx = nullptr;
	
	std::unique_ptr<struct ScopeFpsLock> FpsLock;

	TUserRenderCallback UserRenderCallback;

	std::vector<std::function<void()>> Tasks;
	std::mutex TaskMutex;

public:

	App();

	~App();

	void Run(TUserRenderCallback userRenderCallback);

	static App* GetInstance()
	{
		return s_instance;
	}

	GLFWwindow* GetWindow() const
	{
		return Window;
	}

	void DispatchInMainThread(const std::function<void()>& task);

private:

	void CreateGLFWObjects();

	void CreateImGui();

	void ClearDisplay() const;

	void UpdateWindow();
};