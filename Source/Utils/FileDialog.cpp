#include "Core.h"
#include "FileDialog.h"

#include "App.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#if defined(WIN32)
#include <commdlg.h>
#endif

using namespace Utils;

#if defined(WIN32)
struct SWin32FileDialog
{
	GLFWwindow* Window;
	
	const char* Filter;

	SWin32FileDialog(GLFWwindow* win, const char* filter)
		: Window(win)
		, Filter(filter)
	{
		
	}

	SOpenFileResult operator()() const
	{
		const HWND hwndOwner = glfwGetWin32Window(Window);

		char szFile[260];
		OPENFILENAME ofn = {};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwndOwner;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = Filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		const bool ok = GetOpenFileNameA(&ofn);

		SOpenFileResult payload;
		payload.Code = EnumFileDialogCode::Ok;

		if (ok)
		{
			payload.Path = ofn.lpstrFile;
		}
		else
		{
			const DWORD errorCode = CommDlgExtendedError();
			if (errorCode)
			{
				payload.ErrorStr = (std::string("Error Code: ") + std::to_string(errorCode));
				payload.Code = EnumFileDialogCode::Failed;
			}
			else
			{
				payload.Code = EnumFileDialogCode::Canceled;
			}
		}

		return payload;
	}
};
#endif

OpenFileFuture Utils::OpenFileDialog(const char* filter)
{
	GLFWwindow* wnd = App::GetInstance()->GetWindow();

#if defined(WIN32)
		return std::async(std::launch::async, SWin32FileDialog(wnd, filter));
#endif
}
