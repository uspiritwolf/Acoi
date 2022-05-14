#include "Core.h"
#include "FileDialog.h"

#include "App.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#if defined(WIN32)
#include <commdlg.h>
#endif


FileDialog::FileDialog(const OnOpenFile& callback, const char* filter)
{
	GLFWwindow* wnd = App::GetInstance()->GetWindow();

	auto bgTask = [wnd, filter, callback]()
	{
#if defined(WIN32)
		const HWND hwndOwner = glfwGetWin32Window(wnd);

		char szFile[260];
		OPENFILENAME ofn = {};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwndOwner;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		const bool ok = GetOpenFileNameA(&ofn);

		std::string error;
		std::string path;
		auto result = FileDialogResult::Ok;

		if (ok)
		{
			path = ofn.lpstrFile;
		}
		else
		{
			const DWORD errorCode = CommDlgExtendedError();
			if (errorCode)
			{
				error = std::string("Error Code: ") + std::to_string(errorCode);
				result = FileDialogResult::Failed;
			}
			else
			{
				result = FileDialogResult::Canceled;
			}
		}

		auto task = [callback, result, error, path]()
		{
			if (callback)
			{
				callback(result, path, error);
			}
		};

		App::GetInstance()->DispatchInMainThread(task);
#endif
	};

	Background = std::thread(bgTask);
}

FileDialog::~FileDialog()
{
	if (Background.joinable())
	{
		Background.join();
	}
}

FileDialog FileDialog::OpenFile(const OnOpenFile& callback, const char* filter)
{
	if (!callback)
		return {};
	return { callback, filter };
}
