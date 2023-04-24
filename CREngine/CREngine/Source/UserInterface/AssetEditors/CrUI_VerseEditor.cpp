#include "CrUI_VerseEditor.h"
#include "CrGlobals.h"
#include "CrApp.h"
#include "CrVerse.h"

//Matching UI style helpers
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"
#include "CrComponentEditor.inl"

#include <UserInterface/CrUI_AssetList.h>

REGISTER_CLASS(CrUI_VerseEditor);

ADD_UI_EDITOR(CrVerse, CrUI_VerseEditor);

CrUI_VerseEditor::CrUI_VerseEditor()
{
}

CrUI_VerseEditor::~CrUI_VerseEditor()
{

}

void CrUI_VerseEditor::DrawUI()
{
	Super::DrawUI();
	ImGui::Begin(WindowTitle, &bOpen);

	auto AssetList = GetEditedAsset<CrVerse>();

	if (AssetList.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Add Object Instance"))
	{
		ImGui::OpenPopup("SpawnInstance");
	}

	for (auto& Elem : AssetList->Objects)
	{
		if (ImGui::CollapsingHeader(Elem->GetID().GetStringPretty()))
		{
			EditInstance(Elem, SharedThis<CrUI_VerseEditor>());
		}
	}

	bool bModalOpen = true;
	if (ImGui::BeginPopup("SpawnInstance"/*, &bModalOpen*/))
	{
		if (PopupList == nullptr)
		{
			CrApp* App = CrGlobals::GetAppPointer();
			PopupList = App->MakeUI<CrUI_AssetList, true>();
			PopupList->SetOnOpenAction([&](const Path& DirPath)
				{
					CrSerialization& Serializer = CrSerialization::Get();
					CrObjectFactory& ObjectFactory = CrObjectFactory::Get();
					
					if (CrClass* Class = ObjectFactory.GetClass(Serializer.PeekFile(DirPath).ClassID))
					{
						if (Class->HasFlag(CrClassFlags_Instanceable))
						{
							if (ImGui::Button("Spawn Asset"))
							{
								auto Loaded = Serializer.Load(DirPath);
								Loaded->Rename(CrObjectIDRegistry::CreateUniqueID(Loaded->GetID().GetString().data()));
								if (Loaded)
								{
									AssetList->Objects.emplace_back(Loaded);
									bModalOpen = false;
								}
							}
						}
					}
				});
			//Make same title to ensure it is inside this window.
			PopupList->WindowTitle = "SpawnInstance";
		}

		PopupList->DrawUI();

		if (!bModalOpen)
		{
			PopupList.reset();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}
