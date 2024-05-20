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
class DgCharacter : public CrManagedObject, public CrTick
{
	DEF_CLASS(DgCharacter, CrManagedObject);
public:
	CrComponent<CrRenderable, "Renderable"> Renderable;
	CrComponent<CrSoundPlayer, "SoundPlayer"> SoundPlayer;

	SP<KeySubscriber> UpKeySubscriber;
	SP<KeySubscriber> DownKeySubscriber;
	SP<KeySubscriber> LeftKeySubscriber;
	SP<KeySubscriber> RightKeySubscriber;

	Vec2 CurSpeed = {0.,0.};
	Real DecelPerSec = 0.1;
	Real MaxSpeed = 1.;

	DgCharacter();
	virtual ~DgCharacter();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	virtual void Start() override;
	// Inherited via CrTick
	virtual void DoTick(Seconds DeltaTime) override;

	void AddMovement(const Vec2& Direction);
	void DoMovement(Seconds DeltaTime);
};
