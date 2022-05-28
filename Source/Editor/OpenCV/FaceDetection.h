#pragma once

#include "Editor/EditorInterface.h"
#include "Graphics/Texture.h"

#include "Utils/FileDialog.h"

struct SCascadeData;

struct SDetectionResult
{
	cv::Mat Image;
};

class EFaceDetection : public EditorInterface
{
	bool Visibility = false;

	std::unique_ptr<SCascadeData> CascadeData;

	std::string LastError;

	std::string CascadePath;

	std::string NestedCascadePath;

	std::string ImagePath;

	Utils::OpenFileFuture CascadeOpenFileFuture;

	Utils::OpenFileFuture NestedCascadeOpenFileFuture;

	Utils::OpenFileFuture ImageOpenFileFuture;

	std::future<SDetectionResult> DetectionFuture;

	Texture ImageTex;

public:

	EFaceDetection();

	~EFaceDetection() override = default;

	void Update() override;

	void Show()override;

	void Hide() override;

	bool IsVisible() override;

private:

	void Render();

	void RenderImage() const;

	void RenderError();

	void DetectFace();
};