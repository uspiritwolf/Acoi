#include "Texture.h"

#include <opencv2/imgproc.hpp>

#include "imgui_impl_opengl3_loader.h"

Texture::Texture()
	: Id(0)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &Id);
}

void Texture::LoadBGR(const cv::Mat& image)
{
	cv::Mat rgbImage;
	cvtColor(image, rgbImage, cv::COLOR_BGR2RGBA);
	LoadRGBA(rgbImage);
}

void Texture::LoadRGBA(const cv::Mat& image)
{
	if(!IsValid())
	{
		AllocateResource();
	}

	GLint lastTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glBindTexture(GL_TEXTURE_2D, Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	glBindTexture(GL_TEXTURE_2D, lastTexture);
}

void Texture::AllocateResource()
{
	glGenTextures(1, &Id);
}
