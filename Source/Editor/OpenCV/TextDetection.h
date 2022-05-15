#pragma once

#include "Editor/EditorInterface.h"

#include "Utils/FileDialog.h"
#include "Graphics/Texture.h"

struct SDnnData;

class ETextDetection : public EditorInterface
{
	bool Visibility = false;

	std::unique_ptr<SDnnData> DnnData;

	std::string DetectorModelPath;

	std::string RecognizerModelPath;

	std::string ImagePath;

	FileDialog FdInstance;

	std::string LastError;

	Texture ImageTex;

	std::string RecognizedText;

public:

	ETextDetection();

	void Render() override;

	void Show() override;

	void Hide() override;

	bool IsVisible() override;

private:

	void RenderError();

	void RenderImage();

	void LoadImgEx(const std::string& imagePath);

	void LoadImg(const std::string& imagePath);
};