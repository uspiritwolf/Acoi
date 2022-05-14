/*
 * Code based on https://docs.opencv.org/4.5.5/db/da4/samples_2dnn_2text_detection_8cpp-example.html
 */
#include "Core.h"
#include "TextDetection.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

using namespace cv;
using namespace cv::dnn;

struct SDnnData
{
	TextDetectionModel_EAST detector;
	TextRecognitionModel recognizer;

	SDnnData(const std::string& detModelPath, const std::string& recModelPath)
		: detector(detModelPath)
		, recognizer(recModelPath)
	{
	}
};

ETextDetection::ETextDetection()
{
}

void ETextDetection::Render()
{
	if (!ImGui::Begin("Text Detection", &Visibility))
		ImGui::End();



	ImGui::End();
}

void ETextDetection::Show()
{
	Visibility = true;
}

void ETextDetection::Hide()
{
	Visibility = false;
}

bool ETextDetection::IsVisible()
{
	return Visibility;
}

/*
void ETextDetection::InitDnnData()
{
	DnnData.reset(new SDnnData("", ""));
	DataInitialized = true;
}
*/
