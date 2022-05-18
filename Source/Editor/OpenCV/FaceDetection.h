#pragma once

#include "Editor/EditorInterface.h"

struct SCascadeData;

class EFaceDetection : public EditorInterface
{
	bool Visibility = false;

	std::unique_ptr<SCascadeData> CascadeData;

	std::string CascadePath;

	std::string NestedCascadePath;

public:

	EFaceDetection() = default;

	~EFaceDetection() override = default;

	void Update() override;

	void Show()override;

	void Hide() override;

	bool IsVisible() override;
};