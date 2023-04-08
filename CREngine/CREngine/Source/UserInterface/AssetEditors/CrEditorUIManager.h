#pragma once
#include "CrLoadable.h"

class CrManagedObject;
class CrUI_Editor_AssetBase;

//Map of a type of asset to the UI that can edit it.
class CrEditorUIManager
{
	friend struct CrAddEditor;
	Map<ClassGUID, ClassGUID> AssetToUIClass;

	//Should only edit things from one place at a time.
	Map<HWP<CrManagedObject>, WP<CrUI_Editor_AssetBase>> ExistingEditors;
public:
	static CrEditorUIManager& Get();

	bool MakeEditUI(CrLoadable<CrManagedObject> Object);
	bool HasEditUI(ClassGUID Class);
	ClassGUID FindUIClass(ClassGUID Class);
};

//Helper struct to add a new class -> UI binding
struct CrAddEditor
{
	CrAddEditor(ClassGUID ClassID, ClassGUID InEditorUIClass)
	{
		CrEditorUIManager::Get().AssetToUIClass.emplace(ClassID, InEditorUIClass);
	}
};

//Adds a UI editor mapping
#define ADD_UI_EDITOR(EditorTargetClass, EditorClass) static CrAddEditor EditorTargetClass##_AddEditor_##EditorClass = CrAddEditor(##EditorTargetClass::StaticClass(), ##EditorClass::StaticClass())
