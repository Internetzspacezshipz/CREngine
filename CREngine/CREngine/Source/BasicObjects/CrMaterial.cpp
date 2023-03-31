#include "CrMaterial.h"
#include "CrGlobals.h"
#include "CrShader.h"

REGISTER_CLASS(CrMaterial);

CrMaterial::~CrMaterial()
{
	UnloadMaterial();
}

void CrMaterial::BinSerialize(CrArchive& Arch)
{
	Arch <=> VertexShader;
	Arch <=> FragmentShader;

	if (Arch.bSerializing == false)
	{
		UnloadMaterial();
		LoadMaterial();
	}
}

//void CrMaterial::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	//Super::Serialize(bSerializing, TargetJson);
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, VertexShader);
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FragmentShader);
//
//	if (!bSerializing)
//	{
//		UnloadMaterial();
//		LoadMaterial();
//	}
//}

bool CrMaterial::LoadMaterial()
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
			VulkanEngine* Engine = CrGlobals::GetEnginePointer();

			Engine->MakeDefaultPipeline(VShader, FShader, MaterialData);

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
	if (MaterialData.pipeline)
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		Engine->DestroyMaterial(MaterialData);
	}
}
