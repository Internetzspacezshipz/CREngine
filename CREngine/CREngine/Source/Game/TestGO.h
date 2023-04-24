#pragma once
#include "CrManagedObject.h"

#include "CrComponent.h"
#include <BasicObjects/CrRenderable.h>
#include <BasicObjects/CrSoundPlayer.h>
#include "UserInterface/AssetEditors/CrUI_Editor_AssetBase.h"
#include "CrTickSystem.h"
#include "Utilities/CrGrid.h"

//A 2d object that is renderable in the scene.
//Todo: remove RenderObject inheritance and turn it to composition instead.
class TestGO : public CrManagedObject, public CrTick, public CrGridCell
{
	DEF_CLASS(TestGO, CrManagedObject);

	CrComponent<CrRenderable, "Renderable"> Renderable;
	CrComponent<CrSoundPlayer, "SoundPlayer"> SoundPlayer;

	TestGO();
	virtual ~TestGO();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	// Inherited via CrTick
	virtual void DoTick(Seconds DeltaTime) override;
};


class UI_TestGO : public CrUI_Editor_AssetBase
{
	DEF_CLASS(UI_TestGO, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;
};