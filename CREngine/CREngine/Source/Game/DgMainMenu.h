#pragma once
#include "UserInterface/CrUI_Base.h"

#include "CrComponent.h"
#include <BasicObjects/CrRenderable.h>
#include <BasicObjects/CrSoundPlayer.h>
#include "UserInterface/AssetEditors/CrUI_Editor_AssetBase.h"
#include "CrTickSystem.h"
#include "Utilities/CrGrid.h"

//A 2d object that is renderable in the scene.
//Todo: remove RenderObject inheritance and turn it to composition instead.
class DgMainMenu : public CrUI_Base
{
	DEF_CLASS(DgMainMenu, CrUI_Base);

	DgMainMenu();
	virtual ~DgMainMenu();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	virtual void DrawUI() override;
};
