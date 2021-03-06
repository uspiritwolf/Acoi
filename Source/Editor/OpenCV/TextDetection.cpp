/*
 * Code based on https://docs.opencv.org/4.5.5/db/da4/samples_2dnn_2text_detection_8cpp-example.html
 *
 * Text detection model: https://github.com/argman/EAST
 * Download link: https://www.dropbox.com/s/r2ingd0l3zt8hxs/frozen_east_text_detection.tar.gz?dl=1
 *
 * Text recognition models can be downloaded directly here:
 * Download link: https://drive.google.com/drive/folders/1cTbQ3nuZG-EKWak6emD_s8_hHXWz7lAr?usp=sharing
 */
#include "Core.h"
#include "TextDetection.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace cv::dnn;

struct SDnnData
{
	TextDetectionModel_EAST		detector;
	TextRecognitionModel		recognizer;

	SDnnData(const std::string& detModelPath, const std::string& recModelPath)
		: detector(detModelPath)
		, recognizer(recModelPath)
	{
	}

	void UpdateConfig(float confidenceThreshold, float nmsThreshold)
	{
		detector.setConfidenceThreshold(confidenceThreshold).setNMSThreshold(nmsThreshold);

		recognizer.setVocabulary({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
			, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m"
			,"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"});

		recognizer.setDecodeType("CTC-greedy");

		constexpr double recScale = 1.0 / 127.5;
		const Scalar recMean = Scalar(127.5, 127.5, 127.5);
		const Size recInputSize = Size(100, 32);
		recognizer.setInputParams(recScale, recInputSize, recMean);

		constexpr double detScale = 1.0;
		const Size detInputSize = Size(320, 320);
		const Scalar detMean = Scalar(123.68, 116.78, 103.94);
		const bool swapRB = true;
		detector.setInputParams(detScale, detInputSize, detMean, swapRB);

	}
};

ETextDetection::ETextDetection()
{
}

void ETextDetection::Update()
{
	if(ImageOpenFileFuture.valid() && ImageOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = ImageOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			ImagePath = result.Path;
		}
	}

	if (RecognizerOpenFileFuture.valid() && RecognizerOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = RecognizerOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			RecognizerModelPath = result.Path;
		}
	}

	if (DetectorOpenFileFuture.valid() && DetectorOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = DetectorOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			DetectorModelPath = result.Path;
		}
	}

	if(RecognitionFuture.valid() && RecognitionFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		try
		{
			const auto result = RecognitionFuture.get();
			ImageTex.LoadBGR(result.Image);
			RecognizedText = result.RecognizedText;
		}
		catch(const std::exception& ex)
		{
			LastError = ex.what();
		}
	}

	Render();
}

void ETextDetection::Render()
{
	if (!ImGui::Begin("Text Detection", &Visibility, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Download models"))
			{
				if (ImGui::MenuItem("Detection text (.pb)"))
				{
					system(R"(start https://www.dropbox.com/s/r2ingd0l3zt8hxs/frozen_east_text_detection.tar.gz?dl=1)"); // not sure that it work for another OS
				}

				if (ImGui::MenuItem("Recognition text (.onnx)"))
				{
					system(R"(start https://drive.google.com/drive/folders/1cTbQ3nuZG-EKWak6emD_s8_hHXWz7lAr?usp=sharing)"); // not sure that it work for another OS
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	{
		ImGui::BeginDisabled(DnnData != nullptr);
		if (ImGui::Button("Select##1"))
		{
			DetectorOpenFileFuture = Utils::OpenFileDialog( "Detector model (.pb)\0*.pb\0");
		}

		ImGui::SameLine(); ImGui::InputTextWithHint("Detector Model", "Select .pb file", &DetectorModelPath, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::Button("Select##2"))
		{
			RecognizerOpenFileFuture = Utils::OpenFileDialog("Recognition model (.onnx)\0*.onnx\0");
		}
		ImGui::SameLine(); ImGui::InputTextWithHint("Recognizer Model", "Select .onnx file", &RecognizerModelPath, ImGuiInputTextFlags_ReadOnly);
		ImGui::EndDisabled();

		if (DnnData)
		{
			if (ImGui::Button("Unload DNN"))
			{
				DnnData.reset();
			}
		}
		else
		{
			if (ImGui::Button("Load DNN"))
			{
				SDnnData* data = {};

				try
				{
					data = new SDnnData(DetectorModelPath, RecognizerModelPath);
				}
				catch(const std::exception& ex)
				{
					LastError = ex.what();
				}

				if (data)
				{
					DnnData.reset(data);
				}
			}
		}
	}

	ImGui::Separator();

	if(ImGui::CollapsingHeader("Detection Config"))
	{
		ImGui::DragFloat("Detection confidence threshold", &ConfidenceThreshold, 0.01f);
		ImGui::DragFloat("Detection NMS filter threshold", &NmsFilterThreshold, 0.01f);
	}

	ImGui::Separator();

	{
		ImGui::BeginDisabled(DnnData == nullptr || RecognitionFuture.valid() && RecognitionFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout);

		if (ImGui::Button("Select File##3"))
		{
			ImageOpenFileFuture = Utils::OpenFileDialog("Image\0*.jpg;*.png;*.jpeg;*.tiff\0");
		}
		ImGui::SameLine(); ImGui::InputText("Image", &ImagePath, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();

		if (ImGui::Button("Recognize"))
		{
			RecognitionFuture = LoadAndRecognize(ImagePath);
		}

		ImGui::EndDisabled();
	}

	{
		if (RecognitionFuture.valid() && RecognitionFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
		{
			ImGui::ProgressBar(Progress);
		}
		RenderImage();
	}

	ImGui::End();

	RenderError();
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

void ETextDetection::RenderError()
{
	if (LastError.empty())
		return;

	ImGui::OpenPopup("Error");

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(800, 0), ImVec2(800, -1));

	if(ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", LastError.c_str());

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); LastError.clear(); }
		ImGui::SetItemDefaultFocus();

		ImGui::EndPopup();
	}
}

void ETextDetection::RenderImage()
{
	if(ImageTex.IsValid())
	{
		ImGui::Separator();
		ImGui::Image(ImageTex, ImVec2(500, 500));
		ImGui::SameLine();
		ImGui::InputTextMultiline("###Result", &RecognizedText, ImVec2(250, 500), ImGuiInputTextFlags_ReadOnly);
	}
}

std::future<SRecognitionResult> ETextDetection::LoadAndRecognize(const std::string& imagePath)
{
	Progress = 0.0f;

	if (DnnData == nullptr)
		return {};

	DnnData->UpdateConfig(ConfidenceThreshold, NmsFilterThreshold);

	auto task = [this, imagePath]()
	{
		Mat frame = imread(imagePath, IMREAD_COLOR);

		std::vector< std::vector<Point> > detResults;
		DnnData->detector.detect(frame, detResults);

		{
			struct ComparePoints
			{
				bool operator()(const Point& a, const Point& b) const
				{
					return (a.y == b.y) ? (a.x < b.x) : (a.y < b.y);
				}
			};

			struct CompareQuadrangles
			{
				double DistanceThreshold = 1.0;

				CompareQuadrangles(double distanceThreshold)
					: DistanceThreshold(distanceThreshold)
				{}

				bool operator()(const std::vector<Point>& a, const std::vector<Point>& b) const
				{
					const double yCenterA = (a[0].y + a[2].y) * 0.5;
					const double yCenterB = (b[0].y + b[2].y) * 0.5;
					const bool compareByX = std::abs(yCenterA - yCenterB) < DistanceThreshold;
					return (compareByX) ? (a[0].x < b[0].x) : (a[0].y < b[0].y);
				}
			};

			double averageHeight = 0.0;
			for (auto points : detResults)
			{
				std::sort(points.begin(), points.end(), ComparePoints());
				averageHeight += std::abs(points[2].y - points[0].y);
			}
			averageHeight = averageHeight / static_cast<double>(detResults.size());

			std::sort(detResults.begin(), detResults.end(), CompareQuadrangles(averageHeight * 0.5));
		}

		std::stringstream stringStream;

		if (!detResults.empty())
		{
			int countProgress = 0;

			Mat recInput;
			cvtColor(frame, recInput, cv::COLOR_BGR2GRAY);

			auto fourPointsTransform = [](const Mat& frame, const Point2f vertices[], Mat& result)
			{
				const Size outputSize = Size(100, 32);
				const Point2f targetVertices[4] = {
					Point(0, outputSize.height - 1),
					Point(0, 0), Point(outputSize.width - 1, 0),
					Point(outputSize.width - 1, outputSize.height - 1)
				};
				const Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);
				warpPerspective(frame, result, rotationMatrix, outputSize);
			};

			float prevY = 0.0f;
			for (auto& quadrangle : detResults)
			{
				std::vector<Point2f> quadrangle_2f;
				quadrangle_2f.reserve(4);
				for (int j = 0; j < 4; j++)
					quadrangle_2f.emplace_back(quadrangle[j]);

				Mat cropped;
				fourPointsTransform(recInput, &quadrangle_2f[0], cropped);

				if (prevY > 0)
				{
					if (quadrangle_2f[1].y > prevY)
					{
						stringStream << "\n";
					}
				}
				prevY = quadrangle_2f[3].y;

				stringStream << DnnData->recognizer.recognize(cropped) << " ";
				Progress = static_cast<float>(countProgress) / detResults.size();
				countProgress++;
			}
			polylines(frame, detResults, true, Scalar(0, 255, 0), 2);
		}

		SRecognitionResult result;
		result.RecognizedText = stringStream.str();
		result.Image = std::move(frame);
		return result;
	};

	return std::async(std::launch::async, task);
}
