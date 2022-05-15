#pragma once

enum class FileDialogResult
{
	Failed,
	Canceled,
	Ok
};

using FilePathString = std::string;

using FileErrorString = const std::string;

using OnOpenFile = std::function<void(FileDialogResult, const FilePathString&, FileErrorString&)>;

class FileDialog
{
	std::thread Background;

public:

	FileDialog() = default;

	FileDialog(const OnOpenFile& callback, const char* filter);

	FileDialog(const FileDialog& copy) = delete;

	FileDialog& operator=(const FileDialog& copy) = delete;

	FileDialog& operator=(FileDialog&& move) noexcept
	{
		std::swap(Background, move.Background);
		return *this;
	}

	FileDialog(FileDialog&& move) noexcept
	{
		std::swap(Background, move.Background);
	}

	~FileDialog();

	static FileDialog OpenFile(const OnOpenFile& callback, const char* filter = "All Files\0*.*\0");
};
