#include "CRE_UI_AssetListEditor.hpp"
#include "CRE_Globals.hpp"
#include "CRE_App.hpp"
#include "CRE_AssetList.hpp"
#include <stdlib.h>

REGISTER_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

void CRE_UI_AssetListEditor::DrawUI()
{
	if (CurrentAssetList == nullptr)
	{
		CRE_App* App = CRE_Globals::GetAppPointer();

		CurrentAssetList = App->GetRootAssetList();
	}

	std::string FileName = CurrentAssetList->AssetListPath.filename().generic_string();
	std::string FileNameCpy = FileName;

	ImGui::Begin("AssetListEditor", &bIsOpen);

	ImGui::InputText("Filename:", &FileName);

	if (FileName != FileNameCpy)
	{
		CurrentAssetList->AssetListPath = FileName;
	}
	ImGui::End();
}
