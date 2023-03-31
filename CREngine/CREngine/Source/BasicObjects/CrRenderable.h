#pragma once

#include "CrManagedObject.hpp"
#include "CrMath.hpp"
#include "CrLoadable.hpp"

#include "CrMesh.h"
#include "CrMaterial.h"

#include "vk_engine.h"

struct Mesh;
struct MaterialData;

//A 2d object that is renderable in the scene used for UI mainly.
class CrRenderable : public CrManagedObject, public RenderObject, public std::enable_shared_from_this<CrRenderable>
{
	DEF_CLASS(CrRenderable, CrManagedObject);

	virtual ~CrRenderable();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	void SetRenderingEnabled(bool bRenderingEnabled);
	void LoadRenderable();
	void UnloadRenderable();

	CrLoadable<CrMaterial> MaterialOb;

	CrLoadable<CrMesh> MeshOb;

	// Inherited via RenderObject
	virtual Mesh* GetMesh() override;
	virtual MaterialData* GetMaterial() override;

private:
	bool bHasBeenLoaded = false;
	bool bRenderEnable = true;

};


