#include "Core.h"
#include "FaceDetection.h"

#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/gapi/imgproc.hpp>

using namespace cv;

struct SCascadeData
{
	CascadeClassifier Cascade;

	CascadeClassifier NestedCascade;

	void Load(const std::string& cascadePath, const std::string& nestedCascadePath)
	{
		if(!Cascade.load(samples::findFile(cascadePath)))
		{
			throw std::runtime_error("Could not load classifier cascade");
		}

		if (!NestedCascade.load(samples::findFile(nestedCascadePath)))
		{
			throw std::runtime_error("Could not load classifier cascade for nested objects");
		}
	}
};

EFaceDetection::EFaceDetection()
{
}

void EFaceDetection::Update()
{
	if (CascadeOpenFileFuture.valid() && CascadeOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = CascadeOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			CascadePath = result.Path;
		}
	}

	if (NestedCascadeOpenFileFuture.valid() && NestedCascadeOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = NestedCascadeOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			NestedCascadePath = result.Path;
		}
	}

	if (ImageOpenFileFuture.valid() && ImageOpenFileFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		const Utils::SOpenFileResult result = ImageOpenFileFuture.get();
		if (result.Code == Utils::EnumFileDialogCode::Ok)
		{
			ImagePath = result.Path;
		}
	}

	if (DetectionFuture.valid() && DetectionFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		try
		{
			const auto result = DetectionFuture.get();
			ImageTex.LoadBGR(result.Image);
		}
		catch (const std::exception& ex)
		{
			LastError = ex.what();
		}
	}

	Render();
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

void EFaceDetection::Render()
{
	if (!ImGui::Begin("Face Detection", &Visibility, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	const bool isLoaded = CascadeData != nullptr;

	ImGui::BeginDisabled(isLoaded);

	if (ImGui::Button("Select##1"))
	{
		CascadeOpenFileFuture = Utils::OpenFileDialog("Cascade data (.xml)\0*.xml\0");
	}
	ImGui::SameLine(); ImGui::InputTextWithHint("Cascade data", "Select .xml file", &CascadePath, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("Select##2"))
	{
		NestedCascadeOpenFileFuture = Utils::OpenFileDialog("Nested cascade data (.xml)\0*.xml\0");
	}
	ImGui::SameLine(); ImGui::InputTextWithHint("Nested Cascade data", "Select .xml file", &NestedCascadePath, ImGuiInputTextFlags_ReadOnly);

	ImGui::EndDisabled();

	if(ImGui::Button(isLoaded ? "Unload" : "Load"))
	{
		if(CascadeData)
		{
			CascadeData.reset();
		}
		else
		{
			CascadeData.reset(new SCascadeData());
			try
			{
				CascadeData->Load(CascadePath, NestedCascadePath);
			}
			catch(std::exception& ex)
			{
				CascadeData.reset();
				throw ex;
			}
		}
	}

	ImGui::Separator();

	ImGui::BeginDisabled(!isLoaded);
	if (ImGui::Button("Select##3"))
	{
		ImageOpenFileFuture = Utils::OpenFileDialog("Image\0*.jpg;*.png;*.jpeg;*.tiff\0");
	}
	ImGui::SameLine(); ImGui::InputText("Image", &ImagePath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	if(ImGui::Button("Detect Face"))
	{
		DetectFace();
	}

	ImGui::Separator();

	RenderImage();

	ImGui::End();

	RenderError();
}

void EFaceDetection::RenderImage() const
{
	if (ImageTex.IsValid())
	{
		ImGui::Separator();
		ImGui::Image(ImageTex, ImVec2(500, 500));
	}
}

void EFaceDetection::RenderError()
{
	if (LastError.empty())
		return;

	ImGui::OpenPopup("Error");

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(800, 0), ImVec2(800, -1));

	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", LastError.c_str());

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); LastError.clear(); }
		ImGui::SetItemDefaultFocus();

		ImGui::EndPopup();
	}
}

void EFaceDetection::DetectFace()
{
	const static std::vector colors =
	{
		Scalar(255,0,0),
		Scalar(255,128,0),
		Scalar(255,255,0),
		Scalar(0,255,0),
		Scalar(0,128,255),
		Scalar(0,255,255),
		Scalar(0,0,255),
		Scalar(255,0,255)
	};

	auto task = [this]() -> SDetectionResult
	{
		Mat resultFrame = imread(ImagePath, IMREAD_COLOR);
		Mat frame;

		cvtColor(resultFrame, frame, COLOR_BGR2GRAY);

		equalizeHist(frame, frame);

		std::vector<Rect> faces;

		CascadeData->Cascade.detectMultiScale(
			frame
			, faces, 1.1, 2, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			| CASCADE_SCALE_IMAGE
			, Size(30, 30)
		);

		size_t colorIndex = 0;
		std::vector<Rect> nestedObjects;
		for(const Rect& rect : faces)
		{
			Scalar color = colors[colorIndex];
			colorIndex = (colorIndex + 1) % colors.size();

			rectangle(resultFrame, Point(cvRound(rect.x), cvRound(rect.y)), Point(cvRound(rect.x + rect.width - 1), cvRound(rect.y + rect.height - 1)), color, 3, 8, 0);

			Mat smallImg = frame(rect);

			nestedObjects.clear();

			CascadeData->NestedCascade.detectMultiScale(smallImg, nestedObjects,
				1.1, 2, 0
				//|CASCADE_FIND_BIGGEST_OBJECT
				//|CASCADE_DO_ROUGH_SEARCH
				//|CASCADE_DO_CANNY_PRUNING
				| CASCADE_SCALE_IMAGE,
				Size(30, 30));

			for (const Rect& nr : nestedObjects)
			{
				const Point center(
					cvRound(rect.x + nr.x + nr.width * 0.5),
					cvRound(rect.y + nr.y + nr.height * 0.5));
				const int radius = cvRound((nr.width + nr.height) * 0.25);
				circle(resultFrame, center, radius, color, 3, 8, 0);
			}
		}

		SDetectionResult result;
		result.Image = resultFrame;
		return result;
	};

	DetectionFuture = std::async(std::launch::async, task);
}
