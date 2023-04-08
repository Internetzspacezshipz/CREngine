#pragma once

#include "CrManagedObject.h"
#include "CrLoadable.h"

#include "CrMesh.h"
#include "CrMaterial.h"

#include "vk_engine.h"

struct MeshData;
struct MaterialData;

//A 2d object that is renderable in the scene used for UI mainly.
//Todo: remove RenderObject inheritance and turn it to composition instead.
class CrRenderable : public CrManagedObject, public RenderObject, public std::enable_shared_from_this<CrRenderable>
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
	virtual MeshData* GetMesh() override;
	virtual MaterialData* GetMaterial() override;

private:
	bool bHasBeenLoaded = false;
	bool bRenderEnable = true;
};


