#pragma once

#include "CrManagedObject.hpp"
#include <vk_engine.h>
#include "CrLoadable.hpp"

#include "CrShader.h"
#include "CrTexture.h"

//A material which can be loaded into the GPU and used in the scene.
class CrMaterial : public CrManagedObject
{
	DEF_CLASS(CrMaterial, CrManagedObject);

	virtual ~CrMaterial();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	// A "material" in vulkan is initialized in the following manner:
	// 1. Load/create shader modules - specifically these are the compiled shader files
	// 2. Build a pipeline for the shaders to be used in. A usual setup will have 2 shader stages, vertex, then fragment
	// 3. Make a PipelineLayout for the shaders to do something idk
	// 3.1 The PipelineLayout uses vertex descriptions and vertex bindings to do shit idk.
	// 4. The PipelineLayout is used to make a pipeline???? idk wtf is going on at this stage.
	// 5. Make an actual Pipeline for stuff... idk
	// 6. A "material" is basically a pipeline + a pipeline layout afaik
	// ?. Destroy the shader modules for some reason? I don't really understand why this is the case.

	bool LoadMaterial();
	void UnloadMaterial();

	CrLoadable<CrShader> VertexShader;
	CrLoadable<CrShader> FragmentShader;
	CrLoadable<CrTexture> Texture;

	//Todo: setup push constants here, since they are shared between vert shader and frag shader in the pipeline.

	MaterialData* GetData() { return &MatData; }

private:
	MaterialData MatData;
};


