#include "CrMaterial.h"
#include "CrGlobals.h"

REGISTER_CLASS_FLAGS(CrMaterial, CrClassFlags_Unique);

//Is this necessary? Not really. But I want to.
REGISTER_EXTENSION(CrMaterial, ".crmat");

CrMaterial::~CrMaterial()
{
	UnloadMaterial();
}

void CrMaterial::BinSerialize(CrArchive& Arch)
{
	Arch <=> VertexShader;
	Arch <=> FragmentShader;
	Arch <=> Texture;

	if (Arch.bSerializing == false)
	{
		UnloadMaterial();
		LoadMaterial();
	}
}

bool CrMaterial::LoadMaterial()
{
	//Already loaded.
	if (MatData.pipeline)
	{
		return true;
	}

	VertexShader.SafeLoad();
	FragmentShader.SafeLoad();
	Texture.SafeLoad();

	if (VertexShader.IsLoaded()
		&& FragmentShader.IsLoaded()
		&& Texture.IsLoaded())
	{
		auto VShader = VertexShader->GetShader();
		auto FShader = FragmentShader->GetShader();
		MatData.textureSet = Texture->GetData()->DescriptorSet;
		if (VShader && FShader)
		{
			VulkanEngine* Engine = CrGlobals::GetEnginePointer();

			Engine->MakeDefaultPipeline(VShader, FShader, MatData);

			//Unload shaders since we won't need them anymore. Maybe one day we will reuse them for other materials, but for now don't bother.
			VertexShader->UnloadShader();
			FragmentShader->UnloadShader();
			return true;
		}
	}
	return false;
}

void CrMaterial::UnloadMaterial()
{
	if (MatData.pipeline)
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		Engine->DestroyMaterial(MatData);
	}
}
