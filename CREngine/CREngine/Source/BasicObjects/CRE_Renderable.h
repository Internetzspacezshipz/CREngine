#pragma once

#include "CRE_ManagedObject.hpp"
#include "CRE_Math.hpp"
#include "CRE_Loadable.hpp"

#include "CRE_Mesh.h"
#include "CRE_Material.h"

#include "vk_engine.h"

struct Mesh;
struct Material;

//A 2d object that is renderable in the scene used for UI mainly.
class CRE_Renderable : public CRE_ManagedObject, public RenderObject, public std::enable_shared_from_this<CRE_Renderable>
{
	DEF_CLASS(CRE_Renderable, CRE_ManagedObject);

	virtual ~CRE_Renderable();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	void SetRenderingEnabled(bool bRenderingEnabled);
	void LoadRenderable();
	void UnloadRenderable();

	CRE_Loadable<CRE_Material> MaterialOb;

	CRE_Loadable<CRE_Mesh> MeshOb;

	// Inherited via RenderObject
	virtual Mesh* GetMesh() override;
	virtual Material* GetMaterial() override;

private:
	bool bHasBeenLoaded = false;
	bool bRenderEnable = true;

};


