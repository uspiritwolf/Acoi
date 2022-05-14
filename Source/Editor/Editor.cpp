#include "Core.h"
#include "Editor.h"

#include "OpenCV/TextDetection.h"

EEditor::EEditor()
	: ChildEditor()
	, TextDetection(CreateChildEditor<ETextDetection>())
{
}

void EEditor::Render()
{
	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("OpenCV"))
		{
			const bool isTextDetectionVisibled = TextDetection->IsVisible();
			if(ImGui::MenuItem("DNN: Text Detection", nullptr, isTextDetectionVisibled))
			{
				isTextDetectionVisibled ? TextDetection->Hide() : TextDetection->Show();
			}

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

	RenderChilds();

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

void EEditor::RenderChilds() const
{
	for(auto& child : ChildEditor)
	{
		if(child->IsVisible())
		{
			child->Render();
		}
	}
}
