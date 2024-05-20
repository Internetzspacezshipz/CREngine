#include "DgCharacter.h"
#include "UserInterface/AssetEditors/CrEditorUIManager.h"
#include "UserInterface/AssetEditors/CrComponentEditor.inl"
#include "UserInterface/AssetEditors/CrFieldEditor.inl"

REGISTER_CLASS_FLAGS(DgCharacter, CrClassFlags_Instanceable);

DgCharacter::DgCharacter() : CrTick(GetClass())
{

}

DgCharacter::~DgCharacter()
{

}

void DgCharacter::Start()
{
	SP<CrManagedObject> WMO = shared_from_this();
	UpKeySubscriber = CrGlobals::GetKeySystemPointer()->BindToKey(SDL_SCANCODE_W);
	DownKeySubscriber = CrGlobals::GetKeySystemPointer()->BindToKey(SDL_SCANCODE_S);
	LeftKeySubscriber = CrGlobals::GetKeySystemPointer()->BindToKey(SDL_SCANCODE_A);
	RightKeySubscriber = CrGlobals::GetKeySystemPointer()->BindToKey(SDL_SCANCODE_D);

	Renderable->Mesh.Set(CrObjectFactory::Get().Create<CrMesh>());
	Renderable->GetMesh()->MakeFromShape(Shape::ShapeQuad);
	Renderable->SetRenderingEnabled(true);

	Renderable->Material.Set(CrAssetReference(CrID::Constant<"data/CrMaterial_0">(), CrMaterial::StaticClass()));
	Renderable->Material.Load();

	Renderable->LoadRenderable();
}

void DgCharacter::BinSerialize(CrArchive& Arch)
{
	Arch <=> Renderable;
	Arch <=> SoundPlayer;

	Renderable->LoadRenderable();

	//TODO simple config files (maybe json again?)
}

void DgCharacter::DoTick(Seconds DeltaTime)
{
	if (UpKeySubscriber.get()->IsPressed())
	{
		AddMovement({ 1.,0. });
	}
	if (DownKeySubscriber.get()->IsPressed())
	{
		AddMovement({ -1.,0. });
	}
	if (LeftKeySubscriber.get()->IsPressed())
	{
		AddMovement({ 0.,-1. });
	}
	if (RightKeySubscriber.get()->IsPressed())
	{
		AddMovement({ 0.,1. });
	}

	DoMovement(DeltaTime);

	//CrLOG("curspeed {} {}", CurSpeed.x, CurSpeed.y);

	//Do deceleration
	//Real ThisDecelPerSec = DecelPerSec * DeltaTime;
	//CurSpeed.x > 0. ? CurSpeed.x -= ThisDecelPerSec : CurSpeed.x += ThisDecelPerSec;
	//CurSpeed.y > 0. ? CurSpeed.y -= ThisDecelPerSec : CurSpeed.y += ThisDecelPerSec;
}

void DgCharacter::AddMovement(const Vec2& Direction)
{
	CurSpeed += Direction;

	//clamp speed if over max speed.
	const Real DLen = glm::length(CurSpeed);
	if (DLen > MaxSpeed)
	{
		CurSpeed = CurSpeed * (MaxSpeed / DLen);
	}
}

void DgCharacter::DoMovement(Seconds DeltaTime)
{
	auto Loc = Renderable->GetLocation();
	Loc += (CurSpeed * DeltaTime);
	Renderable->SetLocation(Loc);
	CrLOG("loc {} {}", Loc.x, Loc.y);

	CurSpeed = { 0., 0. };
}
