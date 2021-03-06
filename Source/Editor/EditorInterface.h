#pragma once

/*
 * The names of inherited classes from EditorInterface must begin with 'E' 
 * Example: ESomeEditor
 */
class EditorInterface
{
public:

	virtual ~EditorInterface() = default;

	virtual void Update() = 0;

	virtual void Show() = 0;

	virtual void Hide() = 0;

	virtual bool IsVisible() = 0;
};

using EditorInterfaceUPtr = std::unique_ptr<EditorInterface>;