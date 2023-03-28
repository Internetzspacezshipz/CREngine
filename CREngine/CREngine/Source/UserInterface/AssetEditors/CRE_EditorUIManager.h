#pragma once
#include "CRE_Loadable.hpp"

class CRE_ManagedObject;
class CRE_UI_Editor_AssetBase;

//Map of a type of asset to the UI that can edit it.
class CRE_EditorUIManager
{
	friend struct CRE_AddEditor;
	Map<ClassGUID, ClassGUID> AssetToUIClass;

	//Should only edit things from one place at a time.
	Map<HWP<CRE_ManagedObject>, WP<CRE_UI_Editor_AssetBase>> ExistingEditors;
public:
	static CRE_EditorUIManager& Get();

	bool MakeEditUI(CRE_Loadable<CRE_ManagedObject> Object);
	bool HasEditUI(ClassGUID Class);
	ClassGUID FindUIClass(ClassGUID Class);
};

//Helper struct to add a new class -> UI binding
struct CRE_AddEditor
{
	CRE_AddEditor(ClassGUID ClassID, ClassGUID InEditorUIClass)
	{
		CRE_EditorUIManager::Get().AssetToUIClass.emplace(ClassID, InEditorUIClass);
	}
};

//Adds a UI editor mapping
#define ADD_UI_EDITOR(EditorTargetClass, EditorClass) static CRE_AddEditor EditorTargetClass##_AddEditor_##EditorClass = CRE_AddEditor(##EditorTargetClass::StaticClass(), ##EditorClass::StaticClass())
