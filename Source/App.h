#pragma once

class App
{
	struct GLFWwindow* Window = nullptr;

	struct ImGuiContext* ImGuiCtx = nullptr;

	struct ImPlotContext* ImPlotCtx = nullptr;

public:

	using TRenderCallback = std::function<void()>;

	App();

	~App();

	void Run(TRenderCallback renderCallback);

private:

	void CreateGLFWObjects();

	void CreateImGui();
};