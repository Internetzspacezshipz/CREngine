#pragma once

#include "CrManagedObject.h"
#include "CrLoadable.h"

#include "CrMesh.h"
#include "CrMaterial.h"

#include "vk_engine.h"

struct MeshData;
struct MaterialData;

//A 2d object that is renderable in the scene.
//Todo: remove RenderObject inheritance and turn it to composition instead.
class CrRenderable : public CrManagedObject
{
	DEF_CLASS(CrRenderable, CrManagedObject);

	virtual ~CrRenderable();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	void SetRenderingEnabled(bool bRenderingEnabled);
	void LoadRenderable();
	void UnloadRenderable();

	CrLoadable<CrMaterial> Material;
	CrLoadable<CrMesh> Mesh;

	// Inherited via RenderObject
	MeshData* GetMesh();
	MaterialData* GetMaterial();

	void SetLocation(const Vec2& NewLocation) { Transform.Translation = NewLocation; }
	Vec2 GetLocation() const { return Transform.Translation; }

	void SetRotation(const float& NewRot) { Transform.Rotation = NewRot; }
	float GetRotation() const { return Transform.Rotation; }

	void SetScale(const Vec2& NewScale) { Transform.Scale = NewScale; }
	Vec2 GetScale() const { return Transform.Scale; }

	CrTransform Transform;
private:
	bool bHasBeenLoaded = false;
	bool bRenderEnable = true;
};


