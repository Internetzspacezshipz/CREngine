#pragma once
#include "UserInterface/CRE_UI_Base.h"
#include "CRE_Loadable.hpp"


class CRE_UI_Editor_AssetBase : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_Editor_AssetBase, CRE_UI_Base);

	virtual void Construct() override;
	virtual void DrawUI() override;

	virtual void RemoveUI(bool bPromptAllowed = false) override;
	forceinline void RemoveUIWithPrompt() { RemoveUI(true); };

	//Call this when the asset has been edited.
	void MarkAssetNeedsSave() { bWantsSave = true; }
	virtual void SaveAsset();

	//Flag for when the asset can be saved, but has not been yet.
	bool bWantsSave = false;

	void SetEditedAsset(CRE_Loadable<CRE_ManagedObject> NewAsset);

	//Dcast to the expected asset type...
	template<typename AsType = CRE_ManagedObject>
	SP<AsType> GetEditedAsset() const
	{
		return DCast<AsType>(CurrentEditedAsset.Get<true>());
	}




	//Put this as private until further notice (who is noticing but me?)
private:
	void PopupUI();

	//Variable for rename operation.
	String ActiveEditFilename;

	CRE_Loadable<CRE_ManagedObject> CurrentEditedAsset;
};



//Map of a type of asset to the UI that can edit it.
class CRE_EditorUIManager
{
	friend struct CRE_AddEditor;
	std::unordered_map<ClassGUID, ClassGUID> AssetToUIClass;
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
