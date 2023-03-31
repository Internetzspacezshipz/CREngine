#pragma once
#include "UserInterface/CrUI_Base.h"
#include "CrLoadable.hpp"


class CrUI_Editor_AssetBase : public CrUI_Base
{
	DEF_CLASS(CrUI_Editor_AssetBase, CrUI_Base);

	virtual void Construct() override;

	virtual void DrawUI() override;

	virtual void RemoveUI(bool bPromptAllowed = false) override;

	//Call this when the asset has been edited.
	void MarkAssetNeedsSave() { bWantsSave = true; }
	void SaveAssetWithPrompts();

	//Flag for when the asset can be saved, but has not been yet.
	bool bWantsSave = false;
	bool bIsSaving = false;

	virtual ImGuiWindowFlags GetWindowFlags();

	void SetEditedAsset(CrLoadable<CrManagedObject> NewAsset);

	void PopupUI();

	//Dcast to the expected asset type...
	template<typename AsType = CrManagedObject>
	SP<AsType> GetEditedAsset() const
	{
		return DCast<AsType>(CurrentEditedAsset.Get());
	}

	//Put this as private until further notice (who is noticing but me?)

private:

	//Variable for rename operation.
	String ActiveEditFilename;

	CrLoadable<CrManagedObject> CurrentEditedAsset;
};

