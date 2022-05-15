#pragma once

#include "Editor/EditorInterface.h"

#include "Utils/FileDialog.h"
#include "Graphics/Texture.h"

struct SDnnData;

struct SRecognitionResult
{
	cv::Mat Image;
	std::string RecognizedText;
};

class ETextDetection : public EditorInterface
{
	bool Visibility = false;

	std::unique_ptr<SDnnData> DnnData;

	std::string DetectorModelPath;

	std::string RecognizerModelPath;

	std::string ImagePath;

	Utils::OpenFileFuture DetectorOpenFileFuture;

	Utils::OpenFileFuture RecognizerOpenFileFuture;

	Utils::OpenFileFuture ImageOpenFileFuture;

	std::string LastError;

	Texture ImageTex;

	std::string RecognizedText;

	std::future<SRecognitionResult> RecognitionFuture;

	float Progress = 0.0f;

	float ConfidenceThreshold = 0.5f;

	float NmsFilterThreshold = 0.4f;

public:

	ETextDetection();

	void Update();

	void Render() override;

	void Show() override;

	void Hide() override;

	bool IsVisible() override;

private:

	void RenderError();

	void RenderImage();

	std::future<SRecognitionResult> LoadAndRecognize(const std::string& imagePath);
};