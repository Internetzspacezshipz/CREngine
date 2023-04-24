#include "TestGO.h"
#include "UserInterface/AssetEditors/CrEditorUIManager.h"
#include "UserInterface/AssetEditors/CrComponentEditor.inl"
#include "UserInterface/AssetEditors/CrFieldEditor.inl"

REGISTER_CLASS_FLAGS(TestGO, CrClassFlags_Instanceable);

TestGO::TestGO() : CrTick(GetClass())
{

}

TestGO::~TestGO()
{

}

void TestGO::BinSerialize(CrArchive& Arch)
{
	Arch <=> Renderable;
	Arch <=> SoundPlayer;
}

void TestGO::DoTick(Seconds DeltaTime)
{
	//CrLOG("Some sorta log with delta time %f", DeltaTime);
}

REGISTER_CLASS(UI_TestGO);

ADD_UI_EDITOR(TestGO, UI_TestGO);

void UI_TestGO::DrawUI()
{
	Super::DrawUI();

	ImGui::Begin(WindowTitle, &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<TestGO>();

	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}
	bool bWasEdited = false;

	auto SThis = DCast<UI_TestGO>(shared_from_this());

	bWasEdited |= EditTick<"Tick">(Casted.get());
	bWasEdited |= EditComponent(Casted->Renderable, SThis);
	bWasEdited |= EditComponent(Casted->SoundPlayer, SThis);

	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
