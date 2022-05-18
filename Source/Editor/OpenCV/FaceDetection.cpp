#include "Core.h"
#include "FaceDetection.h"

#include <opencv2/objdetect.hpp>

using namespace cv;

struct SCascadeData
{
	CascadeClassifier Cascade;
	CascadeClassifier NestedCascade;

	void Load()
	{
		
	}
};

void EFaceDetection::Update()
{
	if(!ImGui::Begin("Face Detection", &Visibility, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}



	ImGui::End();
}

void EFaceDetection::Show()
{
	Visibility = true;
}

void EFaceDetection::Hide()
{
	Visibility = false;
}

bool EFaceDetection::IsVisible()
{
	return Visibility;
}