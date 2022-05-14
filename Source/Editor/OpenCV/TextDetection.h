#pragma once

#include "Editor/EditorInterface.h"

struct SDnnData;

class ETextDetection : public EditorInterface
{
	bool Visibility = false;

	std::unique_ptr<SDnnData> DnnData;

public:

	ETextDetection();

	void Render() override;

	void Show() override;

	void Hide() override;

	bool IsVisible() override;
};