#include "Core.h"
#include "Editor.h"

#include "OpenCV/TextDetection.h"
#include "OpenCV/FaceDetection.h"

EEditor::EEditor()
	: ChildEditor()
	, TextDetection(CreateChildEditor<ETextDetection>())
	, FaceDetection(CreateChildEditor<EFaceDetection>())
{
}

struct SSwitcherVisibility
{
	const char* Name = nullptr;

	EditorInterface* Editor = nullptr;

	SSwitcherVisibility(const char* name, EditorInterface* editor)
		: Name(name)
		, Editor(editor)
	{

	}

	void Render()
	{
		const bool isVisible = Editor->IsVisible();
		if (ImGui::MenuItem(Name, nullptr, isVisible))
		{
			isVisible ? Editor->Hide() : Editor->Show();
		}
	}
};

void EEditor::Update()
{
	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("OpenCV"))
		{
			SSwitcherVisibility("DNN: Text Detection", TextDetection).Render();
			SSwitcherVisibility("DNN: Face Detection", FaceDetection).Render();
			
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("ImGui"))
		{
			if(ImGui::MenuItem("Show Demo ImGui", nullptr, ShowDemoImgui))
			{
				ShowDemoImgui = !ShowDemoImgui;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	UpdateChildren();

	if(ShowDemoImgui)
	{
		ImGui::ShowDemoWindow(&ShowDemoImgui);
	}
}

void EEditor::AddChildEditor(EditorInterfaceUPtr editor)
{
	editor->Hide();
	ChildEditor.emplace_back(std::move(editor));
}

void EEditor::Show()
{
	/* already showed */
}

void EEditor::Hide()
{
	/* can't hide */
}

bool EEditor::IsVisible()
{
	return true; /* always showed */
}

void EEditor::UpdateChildren() const
{
	for(auto& child : ChildEditor)
	{
		if(child->IsVisible())
		{
			child->Update();
		}
	}
}
