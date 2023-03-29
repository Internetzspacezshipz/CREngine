#include "CRE_Material.h"
#include "CRE_Globals.h"
#include "CRE_Shader.h"

REGISTER_CLASS(CRE_Material);

CRE_Material::~CRE_Material()
{
	UnloadMaterial();
}

void CRE_Material::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, VertexShader);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FragmentShader);

	if (!bSerializing)
	{
		UnloadMaterial();
		LoadMaterial();
	}
}

bool CRE_Material::LoadMaterial()
{
	//Already loaded.
	if (MaterialData.pipeline)
	{
		return true;
	}

	VertexShader.SafeLoad();
	FragmentShader.SafeLoad();

	if (VertexShader.IsLoaded()
		&& FragmentShader.IsLoaded())
	{
		auto VShader = VertexShader->GetShader();
		auto FShader = FragmentShader->GetShader();
		if (VShader && FShader)
		{
			VulkanEngine* Engine = CRE_Globals::GetEnginePointer();

			Engine->MakeDefaultPipeline(VShader, FShader, MaterialData);

			//Unload shaders since we won't need them anymore. Maybe one day we will reuse them for other materials, but for now don't bother.
			VertexShader->UnloadShader();
			FragmentShader->UnloadShader();
			return true;
		}
	}
	return false;
}

void CRE_Material::UnloadMaterial()
{
	if (MaterialData.pipeline)
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		Engine->DestroyMaterial(MaterialData);
	}
}
