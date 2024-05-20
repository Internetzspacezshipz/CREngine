#include "DgMainMenu.h"
#include "UserInterface/AssetEditors/CrEditorUIManager.h"
#include "UserInterface/AssetEditors/CrComponentEditor.inl"
#include "UserInterface/AssetEditors/CrFieldEditor.inl"
#include "CrApp.h"

REGISTER_CLASS_FLAGS(DgMainMenu, CrClassFlags_Unique);

DgMainMenu::DgMainMenu()
{

}

DgMainMenu::~DgMainMenu()
{

}

void DgMainMenu::BinSerialize(CrArchive& Arch)
{

}

static const CrID MainCharID = CrID::Constant<"MainCharacterRootObj">();
static const CrID MainWorldID = CrID::Constant<"MainWorldRootObj">();

void DgMainMenu::DrawUI()
{
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	auto* CrApp = CrGlobals::GetAppPointer();

	bool bRemove = false;

	//Game already initialized, show only continue button.
	if (CrApp->RootObjects.contains(MainCharID))
	{
		if (ImGui::Button("Continue"))
		{
			bRemove = true;
		}
	}
	else
	{
		if (ImGui::Button("Play!"))
		{
			CrObjectFactory& ObjectFactory = CrObjectFactory::Get();
			SP<CrManagedObject> Character = ObjectFactory.Create(CrID::Constant<"DgCharacter">(), CrID::Constant<"MainCharacter">());

			CrApp->RootObjects.emplace(MainCharID, Array<SP<CrManagedObject>>{Character});

			bRemove = true;
		}
	}

	if (bRemove)
	{
		RemoveUI();
	}
	
	if (ImGui::Button("Quit"))
	{
		CrGlobals::GetEnginePointer()->bWantsQuit = true;
	}
}
