#include "StdAfx.h"

#include "App.h"

void render()
{
	//ImGui::ShowDemoWindow();

	ImPlot::ShowDemoWindow();

	if (ImGui::Begin("F"))
	{

		static float xs1[1001], ys1[1001];
		for (int i = 0; i < 1001; ++i) {
			xs1[i] = i * 0.001f;
			ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
		}
		static double xs2[1001], ys2[1001];
		for (int i = 0; i < 1001; ++i) {
			xs2[i] = i * 0.001f;
			ys2[i] = 0.5f + 0.5f * cosf(50 * (xs2[i] + (float)ImGui::GetTime() / 10));
		}

		if (ImPlot::BeginPlot("Line Plot")) {
			ImPlot::SetupAxes("t", "f(t)");
			ImPlot::PlotLine("sin(t)", xs1, ys1, 1001);
			//ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("cos(t)", xs2, ys2, 1001);
			ImPlot::EndPlot();
		}
	}
	ImGui::End();
}

#if defined(WIN32)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	App app;
	app.Run(&render);
	return 0;
}