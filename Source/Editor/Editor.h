#pragma once

#include "EditorInterface.h"

class ETextDetection;

using EditorInterfaceUPtr = std::unique_ptr<EditorInterface>;

class EEditor : public EditorInterface
{
	std::vector<EditorInterfaceUPtr> ChildEditor;

	ETextDetection* TextDetection = nullptr;

	bool ShowDemoImgui = false;

public:

	EEditor();

	~EEditor() override = default;

	void Update() override;

	void AddChildEditor(EditorInterfaceUPtr editor);

	template<class Type, typename... Args>
	Type* CreateChildEditor()
	{
		Type* ptr = new Type(Args...);
		AddChildEditor(std::unique_ptr<ETextDetection>(ptr));
		return ptr;
	}

	void Show() override;

	void Hide() override;

	bool IsVisible() override;

private:

	void UpdateChildren() const;
};
