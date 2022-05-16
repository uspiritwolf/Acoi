#include "Core.h"

#include "App.h"
#include "Editor/Editor.h"

#if defined(WIN32)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	App app;
	EEditor editor;
	app.Run([&editor] { editor.Update(); });
	return 0;
}