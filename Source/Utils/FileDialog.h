#pragma once

namespace Utils
{
	enum class EnumFileDialogCode
	{
		None,
		Failed,
		Canceled,
		Ok
	};

	struct SOpenFileResult
	{
		EnumFileDialogCode	Code = EnumFileDialogCode::None;
		std::string			Path;
		std::string			ErrorStr;
	};

	using OpenFileFuture = std::future<SOpenFileResult>;

	OpenFileFuture OpenFileDialog(const char* filter = "All Files\0*.*\0");

}