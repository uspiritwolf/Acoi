#pragma once

#include <opencv2/core/mat.hpp>

class Texture
{
	unsigned int Id;

public:

	Texture();

	~Texture();

	void LoadBGR(const cv::Mat& image);

	void LoadRGBA(const cv::Mat& image);

	bool IsValid() const
	{
		return Id != 0;
	}

	operator ImTextureID() const
	{
		return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(Id));
	}

private:

	void AllocateResource();
};